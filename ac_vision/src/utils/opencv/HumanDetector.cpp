/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "HumanDetector.h"

#include <iostream>
#include <fstream>
#include <ros/package.h>

/// staic variabels
CvUtils HumanDetector::cv_utils;

CascadeClassifier HumanDetector::cascade_classifier_full;
CascadeClassifier HumanDetector::cascade_classifier_lower;
CascadeClassifier HumanDetector::cascade_classifier_pedestrain;
CascadeClassifier HumanDetector::cascade_classifier_upper;

HOGDescriptor HumanDetector::hog ( Size ( 64, 64 ), Size ( 16, 16 ), Size ( 8, 8 ), Size ( 8, 8 ), 9, 1,
                                   -1, HOGDescriptor::L2Hys, 0.2, true, HOGDescriptor::DEFAULT_NLEVELS );

LatentSvmDetector HumanDetector::latent_svm_detector;

CvSVM HumanDetector::svmbrief;
CvSVM HumanDetector::svmbrisk;
CvSVM HumanDetector::svmfreak;
CvSVM HumanDetector::svmhog;
CvSVM HumanDetector::svmsift;
CvSVM HumanDetector::svmsurf;

DenseFeatureDetector HumanDetector::detector ( 1, 1, 1, 8, 3 );
DenseFeatureDetector HumanDetector::freak_detector ( 1, 1, 1, 2, 3 );
DenseFeatureDetector HumanDetector::brief_detector ( 1, 1, 1, 8, 3 );
BriefDescriptorExtractor HumanDetector::brief_extractor ( 64 );
BRISK HumanDetector::brisk_extractor ( 30, 3, 1.0 );
FREAK HumanDetector::freak_extractor ( true, true, 22.0, 4, vector< int > ( ) );
SiftDescriptorExtractor HumanDetector::sift_extractor ( 64, 3, 0.04, 10, 1.6 );
SurfDescriptorExtractor HumanDetector::surf_extractor ( 100, 4, 3, true, false );

bool HumanDetector::is_initialised = false;

HumanDetector::HumanDetector ( void )
{

}

void HumanDetector::initialise ( void )
{
  if ( !is_initialised )
    {
      load_cascade_classifiers ( );
      load_classifiers ( );
      load_latent_svm_detector ( );
//   hog.setSVMDetector ( HOGDescriptor::getDefaultPeopleDetector ( ) );
      hog.setSVMDetector ( get_upperbody_detector ( ) );
      is_initialised = true;
    }
}

vector< Rect > HumanDetector::get_cascade_detected_humans ( const Mat& image, CascadeClassifier cascade_classifier, const Size minsize, const Size maxsize )
{
  Mat gray;
  cvtColor ( image, gray, CV_BGR2GRAY );
  equalizeHist ( gray, gray );

  vector< Rect > detected_humans;
  vector< int > reject_levels;
  vector< double > level_weights;
  double scale_factor = 1.1;
  int min_neighbours = 3;
  int flags = 0;
  Size min_size = Size ( );
  Size max_size = Size ( );
  bool output_rejectLevels = false;
  cascade_classifier.detectMultiScale ( gray, detected_humans, reject_levels, level_weights, scale_factor,
                                        min_neighbours, flags, min_size, max_size, output_rejectLevels );

  cout << "\tReject Levels: ";
  for ( unsigned int i = 0; i < reject_levels.size(); i++ )
    {
      cout << reject_levels[i] << " ";
    }
  cout << endl;
  cout << "\tLevel Weights: ";
  for ( unsigned int i = 0; i < level_weights.size(); i++ )
    {
      cout << level_weights[i] << " ";
    }
  cout << endl;
  cout << "\tDetected humans: " << detected_humans.size() << endl;
  return detected_humans;
}

vector< Rect > HumanDetector::get_cascade_detected_humans_full_body ( const Mat& image )
{
  LOG_DEBUG ( "HumanDetector::get_cascade_detected_humans_full_body: " );
  return get_cascade_detected_humans ( image, cascade_classifier_full, Size ( 14, 28 ), Size ( 48, 96 ) );
}

vector< Rect > HumanDetector::get_cascade_detected_humans_lower_body ( const Mat& image )
{
  LOG_DEBUG ( "HumanDetector::get_cascade_detected_humans_lower_body: " );
  return get_cascade_detected_humans ( image, cascade_classifier_lower, Size ( 19, 23 ), Size ( 65, 78 ) );
}

vector< Rect > HumanDetector::get_cascade_detected_humans_upper_body ( const Mat& image )
{
  LOG_DEBUG ( "HumanDetector::get_cascade_detected_humans_upper_body: " );
  return get_cascade_detected_humans ( image, cascade_classifier_upper, Size ( 22, 18 ), Size ( 75, 61 ) );
}

vector< Rect > HumanDetector::get_cascade_detected_pedestrains ( const Mat& image )
{
  LOG_DEBUG ( "HumanDetector::get_cascade_detected_pedestrains: " );
  return get_cascade_detected_humans ( image, cascade_classifier_pedestrain, Size ( 14, 28 ), Size ( 48, 96 ) );
}

Mat get_hogdescriptor_visu ( Mat& color_origImg, vector<float>& descriptorValues )
{
  float zoomFac = 1;
  Mat visu;
  resize ( color_origImg, visu, Size ( color_origImg.cols * zoomFac, color_origImg.rows * zoomFac ) );

  int cellSize        = 8;
  int gradientBinSize = 9;
  float radRangeForOneBin = M_PI/ ( float ) gradientBinSize; // dividing 180° into 9 bins, how large (in rad) is one bin?

  // prepare data structure: 9 orientation / gradient strenghts for each cell
  int cells_in_x_dir = 64 / cellSize;
  int cells_in_y_dir = 128 / cellSize;

  float*** gradientStrengths = new float**[cells_in_y_dir];
  int** cellUpdateCounter   = new int*[cells_in_y_dir];

  for ( int y = 0; y < cells_in_y_dir; y++ )
    {
      gradientStrengths[y] = new float*[cells_in_x_dir];
      cellUpdateCounter[y] = new int[cells_in_x_dir];
      for ( int x=0; x<cells_in_x_dir; x++ )
        {
          gradientStrengths[y][x] = new float[gradientBinSize];
          cellUpdateCounter[y][x] = 0;
          for ( int bin = 0; bin < gradientBinSize; bin++ )
            {
              gradientStrengths[y][x][bin] = 0.0;
            }
        }
    }

  // nr of blocks = nr of cells - 1
  // since there is a new block on each cell (overlapping blocks!) but the last one
  int blocks_in_x_dir = cells_in_x_dir - 1;
  int blocks_in_y_dir = cells_in_y_dir - 1;

  // compute gradient strengths per cell
  int descriptorDataIdx = 0;

  for ( int blockx = 0; blockx < blocks_in_x_dir; blockx++ )
    {
      for ( int blocky = 0; blocky < blocks_in_y_dir; blocky++ )
        {
          // 4 cells per block ...
          for ( int cellNr = 0; cellNr < 4; cellNr++ )
            {
              // compute corresponding cell nr
              int cellx = blockx;
              int celly = blocky;
              if ( cellNr == 1 )
                {
                  celly++;
                }
              if ( cellNr == 2 )
                {
                  cellx++;
                }
              if ( cellNr == 3 )
                {
                  cellx++;
                  celly++;
                }

              for ( int bin = 0; bin < gradientBinSize; bin++ )
                {
                  float gradientStrength = descriptorValues[ descriptorDataIdx ];
                  descriptorDataIdx++;

                  gradientStrengths[celly][cellx][bin] += gradientStrength;
                } // for (all bins)

              // note: overlapping blocks lead to multiple updates of this sum!
              // we therefore keep track how often a cell was updated,
              // to compute average gradient strengths
              cellUpdateCounter[celly][cellx]++;
            } // for (all cells)
        } // for (all block x pos)
    } // for (all block y pos)

  // compute average gradient strengths
  for ( int celly = 0; celly < cells_in_y_dir; celly++ )
    {
      for ( int cellx = 0; cellx < cells_in_x_dir; cellx++ )
        {
          float NrUpdatesForThisCell = ( float ) cellUpdateCounter[celly][cellx];

          // compute average gradient strenghts for each gradient bin direction
          for ( int bin = 0; bin < gradientBinSize; bin++ )
            {
              gradientStrengths[celly][cellx][bin] /= NrUpdatesForThisCell;
            }
        }
    }

  // draw cells
  for ( int celly = 0; celly < cells_in_y_dir; celly++ )
    {
      for ( int cellx = 0; cellx < cells_in_x_dir; cellx++ )
        {
          int drawX = cellx * cellSize;
          int drawY = celly * cellSize;

          int mx = drawX + cellSize / 2;
          int my = drawY + cellSize / 2;

          rectangle ( visu, Point ( drawX * zoomFac, drawY * zoomFac ), Point ( ( drawX + cellSize ) * zoomFac, ( drawY + cellSize ) * zoomFac ), CV_RGB ( 100, 100, 100 ), 1 );

          // draw in each cell all 9 gradient strengths
          for ( int bin = 0; bin < gradientBinSize; bin++ )
            {
              float currentGradStrength = gradientStrengths[celly][cellx][bin];

              // no line to draw?
              if ( currentGradStrength == 0 )
                {
                  continue;
                }

              float currRad = bin * radRangeForOneBin + radRangeForOneBin / 2;

              float dirVecX = cos ( currRad );
              float dirVecY = sin ( currRad );
              float maxVecLen = cellSize / 2;
              float scale = 2.5; // just a visualization scale, to see the lines better

              // compute line coordinates
              float x1 = mx - dirVecX * currentGradStrength * maxVecLen * scale;
              float y1 = my - dirVecY * currentGradStrength * maxVecLen * scale;
              float x2 = mx + dirVecX * currentGradStrength * maxVecLen * scale;
              float y2 = my + dirVecY * currentGradStrength * maxVecLen * scale;

              // draw gradient visualization
              line ( visu, Point ( x1 * zoomFac, y1 * zoomFac ), Point ( x2 * zoomFac, y2 * zoomFac ), CV_RGB ( 0, 255, 0 ), 1 );
            } // for (all bins)
        } // for (cellx)
    } // for (celly)

  // don't forget to free memory allocated by helper data structures!
  for ( int y = 0; y < cells_in_y_dir; y++ )
    {
      for ( int x = 0; x < cells_in_x_dir; x++ )
        {
          delete[] gradientStrengths[y][x];
        }
      delete[] gradientStrengths[y];
      delete[] cellUpdateCounter[y];
    }
  delete[] gradientStrengths;
  delete[] cellUpdateCounter;

  return visu;

} // get_hogdescriptor_visu

void HumanDetector::visualise_hogdescriptors ( const Mat& image )
{
  vector< float > descriptors;
  hog.compute ( image, descriptors );

  size_t dsize = hog.getDescriptorSize();
  int d = 0; // is going to be used to determine current partion of descriptors
  for ( int y = 0; y <= image.rows - hog.winSize.height; y = y + hog.blockStride.height )
    {
      for ( int x = 0; x <= image.cols - hog.winSize.width; x = x + hog.blockStride.width )
        {
          Rect cur_rect ( x, y, hog.winSize.width, hog.winSize.height );
          Mat cur_win ( image, cur_rect );
          vector< float > cur_descriptors ( descriptors.begin() + ( d * dsize ), descriptors.begin() + ( d * dsize ) + dsize );
          Mat cur_win_visual = get_hogdescriptor_visu ( cur_win, cur_descriptors );
          Mat visualised;
          image.copyTo ( visualised );
          cur_win_visual.copyTo ( visualised ( cur_rect ) );
          imshow ( "HOG Descriptor", visualised );
          waitKey ( 0 );
          d++;
        }
    }
}

void HumanDetector::visualise_hoggradient ( const Mat& image )
{
  Mat gradients;
  Mat angleofs;
  hog.computeGradient ( image, gradients, angleofs, Size(), Size() );
  cout << gradients.channels() << " " << angleofs.channels() << endl;
}

vector< Rect > HumanDetector::get_hog_detected_humans ( const Mat& image )
{
  FileStorage fs;
  string filename = ros::package::getPath ( AC_VISION_NODE );
  filename.append ( SETTINGS_FOLDER );
  filename.append ( "hog_settings.yaml" );
  fs.open ( filename, FileStorage::READ );

  vector< Rect > detected_humans;
  vector< double > detected_weights;
  double hit_threshold = ( double ) fs["hit_threshold"];
  Size win_stride = Size ( );
  Size padding = Size ( );
  double scale = 1.05;
  double final_threshold = ( double ) fs["final_threshold"];
  bool use_meanshift_grouping = ( int ) fs["use_meanshift_grouping"];

  fs.release();

  hog.detectMultiScale ( image, detected_humans, detected_weights, hit_threshold, win_stride,
                         padding, scale, final_threshold, use_meanshift_grouping );

//   vector< Point > search_locations;
//   vector< Point > found_locations;
//   hog.detect ( image, found_locations, detected_weights, hit_threshold, win_stride, padding, search_locations );
//
//   for ( unsigned int i = 0 ; i < found_locations.size(); i++ )
//     {
//       Rect human ( found_locations[i], Size ( 64, 128 ) );
//       detected_humans.push_back ( human );
//     }

  for ( unsigned int i = 0 ; i < detected_weights.size(); i++ )
    {
      cout << detected_weights[i] << ", ";
    }
  cout << "[weights]: " << detected_weights.size() << " [humans]: " << detected_humans.size() << endl;

  return detected_humans;
}

vector< Rect > HumanDetector::get_latent_svm_detected_humans ( const Mat& image )
{
  vector< LatentSvmDetector::ObjectDetection > detections;
  latent_svm_detector.detect ( image, detections, 0 );
  vector< Rect > detected_humans;
  for ( size_t i = 0; i < detections.size ( ); i++ )
    {
      LOG_DEBUG ( "HumanDetector - get_latent_svm_detected_humans: score[" << i << "] = " << detections[i].score );
      if ( detections[i].score > -1.0 )
        {
          detected_humans.push_back ( detections[i].rect );
        }
    }
  return detected_humans;
}

bool HumanDetector::is_person_present ( const Mat& image, const int type )
{
  bool present = false;

  if ( image.rows != image.cols )
    {
      LOG_DEBUG ( "HumanDetector::is_person_present: image is not square." );
      return present;
    }
  Mat resized_image;
  resize ( image, resized_image, Size ( 64, 64 ), 0, 0, INTER_CUBIC );

  switch ( type )
    {
    case ML_BRIEF_SVM:
      present = is_person_present_briefsvm ( resized_image );
      break;
    case ML_BRISK_SVM:
      present = is_person_present_brisksvm ( resized_image );
      break;
    case ML_FREAK_SVM:
      present = is_person_present_freaksvm ( resized_image );
      break;
    case ML_HOG_SVM:
      present = is_person_present_hogsvm ( resized_image );
      break;
    case ML_LATENT_SVM:
      present = is_person_present_latentsvm ( resized_image );
      break;
    case ML_SIFT_SVM:
      present = is_person_present_siftsvm ( resized_image );
      break;
    case ML_SURF_SVM:
      present = is_person_present_surfsvm ( resized_image );
      break;
    default:
      break;
    }

  return present;
}

bool HumanDetector::is_person_present_briefsvm ( const Mat& image )
{
  vector< KeyPoint > keypoints;
  brief_detector.detect ( image, keypoints );
  Mat descriptor;
  brief_extractor.compute ( image, keypoints, descriptor );
  descriptor = descriptor.reshape ( 0, 1 );
  descriptor.convertTo ( descriptor, CV_32FC1 );

  float response = svmbrief.predict ( descriptor, false );
  if ( response == LABEL_POSITIVE )
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool HumanDetector::is_person_present_brisksvm ( const Mat& image )
{
  vector< KeyPoint > keypoints;
  detector.detect ( image, keypoints );
  Mat descriptor;
  brisk_extractor.compute ( image, keypoints, descriptor );
  descriptor = descriptor.reshape ( 0, 1 );

  float response = svmbrisk.predict ( descriptor, false );
  if ( response == LABEL_POSITIVE )
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool HumanDetector::is_person_present_cascade ( const Mat& image )
{
  Mat gray;
  cvtColor ( image, gray, CV_BGR2GRAY );
  equalizeHist ( gray, gray );

  vector< Rect > detected_humans;
  vector< int > reject_levels;
  vector< double > level_weights;
  cascade_classifier_upper.detectMultiScale ( gray, detected_humans, reject_levels, level_weights );

  for ( unsigned int i = 0; i < detected_humans.size ( ); i++ )
    {
      return true;
    }
  return false;
}

bool HumanDetector::is_person_present_freaksvm ( const Mat& image )
{
  vector< KeyPoint > keypoints;
  freak_detector.detect ( image, keypoints );
  Mat descriptor;
  freak_extractor.compute ( image, keypoints, descriptor );
  descriptor = descriptor.reshape ( 0, 1 );

  float response = svmfreak.predict ( descriptor, false );
  if ( response == LABEL_POSITIVE )
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool HumanDetector::is_person_present_hogsvm ( const Mat& image )
{
  vector< float > hog_descriptor;
  hog.compute ( image, hog_descriptor );
  Mat descriptor ( hog_descriptor );
  descriptor = descriptor.reshape ( 0, 1 );

  float response = svmhog.predict ( descriptor, false );
  if ( response == LABEL_POSITIVE )
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool HumanDetector::is_person_present_latentsvm ( const Mat& image )
{
  vector< LatentSvmDetector::ObjectDetection > detections;
  latent_svm_detector.detect ( image, detections, 0 );
  for ( unsigned int i = 0; i < detections.size ( ); i++ )
    {
      if ( detections[i].score >= -1 )
        {
          return true;
        }
    }

  return false;
}

bool HumanDetector::is_person_present_siftsvm ( const Mat& image )
{
  vector< KeyPoint > keypoints;
  detector.detect ( image, keypoints );
  Mat descriptor;
  sift_extractor.compute ( image, keypoints, descriptor );
  descriptor = descriptor.reshape ( 0, 1 );

  float response = svmsift.predict ( descriptor, false );
  if ( response == LABEL_POSITIVE )
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool HumanDetector::is_person_present_surfsvm ( const Mat& image )
{
  vector< KeyPoint > keypoints;
  detector.detect ( image, keypoints );
  Mat descriptor;
  surf_extractor.compute ( image, keypoints, descriptor );
  descriptor = descriptor.reshape ( 0, 1 );

  float response = svmsurf.predict ( descriptor, false );
  if ( response == LABEL_POSITIVE )
    {
      return true;
    }
  else
    {
      return false;
    }
}

void HumanDetector::load_cascade_classifier ( CascadeClassifier& cascade_classifier, const string file_name )
{
  if ( !cascade_classifier.load ( file_name ) )
    {
      LOG_ERROR ( "HumanDetector::load_cascade_classifier: could not load cascade classifier \"" << file_name << "\"" );
      exit ( EXIT_FAILURE );
    }
}

void HumanDetector::load_cascade_classifiers ( void )
{
  string package_path = ros::package::getPath ( AC_VISION_NODE );

  string cascade_name_full;
  cascade_name_full.append ( package_path );
  cascade_name_full.append ( HAAR_CASCADE_FOLDER );
  cascade_name_full.append ( "haarcascade_fullbody.xml" );
  load_cascade_classifier ( cascade_classifier_full, cascade_name_full );

  string cascade_name_lower;
  cascade_name_lower.append ( package_path );
  cascade_name_lower.append ( HAAR_CASCADE_FOLDER );
  cascade_name_lower.append ( "haarcascade_lowerbody.xml" );
  load_cascade_classifier ( cascade_classifier_lower, cascade_name_lower );

  string cascade_name_pedestrain;
  cascade_name_pedestrain.append ( package_path );
  cascade_name_pedestrain.append ( HOG_CASCADE_FOLDER );
  cascade_name_pedestrain.append ( "hogcascade_pedestrians.xml" );
  load_cascade_classifier ( cascade_classifier_pedestrain, cascade_name_pedestrain );

  string cascade_name_upper;
  cascade_name_upper.append ( package_path );
  cascade_name_upper.append ( HAAR_CASCADE_FOLDER );
  cascade_name_upper.append ( "haarcascade_upperbody.xml" );
  load_cascade_classifier ( cascade_classifier_upper, cascade_name_upper );
}

void HumanDetector::load_latent_svm_detector ( void )
{
  string filename = ros::package::getPath ( AC_VISION_NODE );
  filename.append ( LATENT_SVM_FOLDER );
  filename.append ( "person.xml" );
  vector< string > models_filenames ( 1, filename );
  latent_svm_detector.load ( models_filenames );
  if ( latent_svm_detector.empty ( ) )
    {
      LOG_ERROR ( "HumanDetector::load_latent_svm_detector: could not load latent svm detector \"" << models_filenames[0] << "\"" );
      exit ( EXIT_FAILURE );
    }
}

void HumanDetector::load_classifiers ( void )
{
  string package_path = ros::package::getPath ( AC_VISION_NODE );

  string svmbrief_filename;
  svmbrief_filename.append ( package_path );
  svmbrief_filename.append ( CLASSIFIERS_FOLDER );
  svmbrief_filename.append ( "ml_svmbrief_c.yaml" );
  svmbrief.load ( svmbrief_filename.c_str ( ) );

  string svmbrisk_filename;
  svmbrisk_filename.append ( package_path );
  svmbrisk_filename.append ( CLASSIFIERS_FOLDER );
  svmbrisk_filename.append ( "ml_svmbrisk_nu.yaml" );
  svmbrisk.load ( svmbrisk_filename.c_str ( ) );

  string svmfreak_filename;
  svmfreak_filename.append ( package_path );
  svmfreak_filename.append ( CLASSIFIERS_FOLDER );
  svmfreak_filename.append ( "ml_svmfreak_nu.yaml" );
  svmfreak.load ( svmfreak_filename.c_str ( ) );

  string svmhog_filename;
  svmhog_filename.append ( package_path );
  svmhog_filename.append ( CLASSIFIERS_FOLDER );
  svmhog_filename.append ( "ml_svmhog_c.yaml" );
  svmhog.load ( svmhog_filename.c_str ( ) );

  string svmsift_filename;
  svmsift_filename.append ( package_path );
  svmsift_filename.append ( CLASSIFIERS_FOLDER );
  svmsift_filename.append ( "ml_svmsift_c.yaml" );
  svmsift.load ( svmsift_filename.c_str ( ) );

  string svmsurf_filename;
  svmsurf_filename.append ( package_path );
  svmsurf_filename.append ( CLASSIFIERS_FOLDER );
  svmsurf_filename.append ( "ml_svmsurf_c.yaml" );
  svmsurf.load ( svmsurf_filename.c_str ( ) );
}

vector< float > HumanDetector::get_upperbody_detector ( void )
{
  string package_path = ros::package::getPath ( AC_VISION_NODE );
  string hog_name;
  hog_name.append ( package_path );
  hog_name.append ( HOG_DESCRIPTOR_FOLDER );
  hog_name.append ( "hogdescriptor-7000p7000n.dat" );
  vector< float > detector;
  ifstream myfile ( hog_name.c_str() );
  while ( myfile.good() )
    {
      string tmp;
      myfile >> tmp;
      if ( tmp.compare ( "" ) != 0 )
        {
          detector.push_back ( atof ( tmp.c_str() ) );
        }
    }
  myfile.close();
  return detector;
}