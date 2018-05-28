#include "../../src/utils/FileUtils.h"
#include "../../src/utils/StringUtils.h"

#include <iostream>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>

using namespace cv;
using namespace std;

string feature_type;
DenseFeatureDetector detector ( 1, 1, 1, 8, 3 );
DenseFeatureDetector freak_detector ( 1, 1, 1, 8, 3 );
DenseFeatureDetector brief_detector ( 1, 1, 1, 8, 3 );
int nfeatures = 64;
int descriptor_length = 0;

/// classifiers
CvSVM svm;
CvANN_MLP ann_mlp;
CvKNearest knn;
EM em ( EM::DEFAULT_NCLUSTERS, EM::COV_MAT_DIAGONAL, TermCriteria ( CV_TERMCRIT_ITER, ( int ) 1e7, 1e-6 ) );
CvERTrees er_trees;
CvRTrees r_trees;
CvGBTrees gb_trees;
CvBoost boost;
CvDTree d_trees;
CvNormalBayesClassifier nbc;

/// brief
int bytes = 64;
BriefDescriptorExtractor brief_extractor ( bytes );

/// brisk
int thresh = 30;
int octaves = 3;
float pattern_scale = 1.0f;
BRISK brisk_extractor ( thresh, octaves, pattern_scale );

/// freak
bool orientation_normalised = true;
bool scale_normalised = true;
float freak_pattern_scale = 22.0f;
int noctaves = 4;
vector< int > selected_pairs;
FREAK freak_extractor ( orientation_normalised, scale_normalised, freak_pattern_scale, noctaves, selected_pairs );

/// hog
Size win_size ( 64, 64 );
Size block_size ( 16, 16 );
Size block_stride ( 8, 8 );
Size cell_size ( 8, 8 );
int nbins = 9;
int deriv_aperture = 1;
int win_sigma = -1;
int histogram_norm_type = HOGDescriptor::L2Hys;
double l2hys_threshold = 0.2;
bool gamma_correction = true;
int nlevels = HOGDescriptor::DEFAULT_NLEVELS;
HOGDescriptor hog_extractor ( win_size, block_size, block_stride, cell_size, nbins, deriv_aperture,
                              win_sigma, histogram_norm_type, l2hys_threshold, gamma_correction, nlevels );

/// orb
float scale_factor = 1.2f;
int orb_nlevels = 8;
int orb_edge_threshold = 31;
int first_level = 0;
int wta_k = 2;
int score_type = ORB::HARRIS_SCORE;
int patch_size = 3;
OrbDescriptorExtractor orb_extractor ( nfeatures, scale_factor, orb_nlevels, orb_edge_threshold, first_level, wta_k, score_type, patch_size );

/// sift
int noctave_layers = 3;
double contrast_threshold = 0.04;
double edge_threshold = 10;
double sigma = 1.6;
SiftDescriptorExtractor sift_extractor ( nfeatures, noctave_layers, contrast_threshold, edge_threshold, sigma );

/// surf
double hessian_threshold = 100;
int surf_noctaves = 4;
int surf_noctave_layers = 3;
bool extended = true;
bool upright = false;
SurfDescriptorExtractor surf_extractor ( hessian_threshold, surf_noctaves, surf_noctave_layers, extended, upright );

Mat is_person_present_hog ( Mat image )
{
  vector< float > hog_descriptor;
  hog_extractor.compute ( image, hog_descriptor );
  Mat descriptor ( hog_descriptor );
  descriptor = descriptor.reshape ( 0, 1 );
  descriptor.convertTo ( descriptor, CV_32FC1 );
  return descriptor;
}

Mat is_person_present_sift ( Mat image )
{
  // getting the features
  vector< KeyPoint > keypoints;
  detector.detect ( image, keypoints );

  Mat descriptor;
  sift_extractor.compute ( image, keypoints, descriptor );

  descriptor = descriptor.reshape ( 0, 1 );
  descriptor.convertTo ( descriptor, CV_32FC1 );

  return descriptor;
}

Mat is_person_present_surf ( Mat image )
{
  // getting the features
  vector< KeyPoint > keypoints;
  detector.detect ( image, keypoints );

  Mat descriptor;
  surf_extractor.compute ( image, keypoints, descriptor );

  descriptor = descriptor.reshape ( 0, 1 );
  descriptor.convertTo ( descriptor, CV_32FC1 );

  return descriptor;
}

Mat is_person_present_brisk ( Mat image )
{
  // getting the features
  vector< KeyPoint > keypoints;
  detector.detect ( image, keypoints );

  Mat descriptor;
  brisk_extractor.compute ( image, keypoints, descriptor );

  descriptor = descriptor.reshape ( 0, 1 );
  descriptor.convertTo ( descriptor, CV_32FC1 );

  return descriptor;
}

Mat is_person_present_brief ( Mat image )
{
  // getting the features
  vector< KeyPoint > keypoints;
  brief_detector.detect ( image, keypoints );

  Mat descriptor;
  brief_extractor.compute ( image, keypoints, descriptor );

  descriptor = descriptor.reshape ( 0, 1 );
  descriptor.convertTo ( descriptor, CV_32FC1 );

  return descriptor;
}

Mat is_person_present_freak ( Mat image )
{
  // getting the features
  vector< KeyPoint > keypoints;
  freak_detector.detect ( image, keypoints );

  Mat descriptor;
  freak_extractor.compute ( image, keypoints, descriptor );

  descriptor = descriptor.reshape ( 0, 1 );
  descriptor.convertTo ( descriptor, CV_32FC1 );

  return descriptor;
}

Mat is_person_present_all ( Mat image )
{
  Mat hog_descriptor = is_person_present_hog ( image );
  Mat sift_descriptor = is_person_present_sift ( image );
  Mat surf_descriptor = is_person_present_surf ( image );
  Mat brisk_descriptor = is_person_present_brisk ( image );
  Mat brief_descriptor = is_person_present_brief ( image );
  Mat freak_descriptor = is_person_present_freak ( image );

  Mat descriptor ( 1, descriptor_length, CV_32FC1 );
  int column = hog_descriptor.cols;
  hog_descriptor.copyTo ( Mat ( descriptor, Rect ( 0, 0, column, 1 ) ) );
  column += sift_descriptor.cols;
  sift_descriptor.copyTo ( Mat ( descriptor, Rect ( 0, 0, column, 1 ) ) );
  column += surf_descriptor.cols;
  surf_descriptor.copyTo ( Mat ( descriptor, Rect ( 0, 0, column, 1 ) ) );
  column += brisk_descriptor.cols;
  brisk_descriptor.copyTo ( Mat ( descriptor, Rect ( 0, 0, column, 1 ) ) );
  column += brief_descriptor.cols;
  brief_descriptor.copyTo ( Mat ( descriptor, Rect ( 0, 0, column, 1 ) ) );
  column += freak_descriptor.cols;
  freak_descriptor.copyTo ( Mat ( descriptor, Rect ( 0, 0, column, 1 ) ) );

  return descriptor;
}

void extract_features ( Mat& labels, Mat& descriptors, string folder, string output_folder, string type )
{
  vector< string > images_filenames;
  FileUtils::read_directory ( folder, images_filenames );
  int nimages = images_filenames.size ( );
  cout << "Number of " << type << " images: " << nimages << endl;

  if ( nimages == 0 )
    {
      cerr << "Provided folder " << folder << " is empty." << endl;
      exit ( 1 );
    }

  float lable;
  if ( type.compare ( "pos" ) == 0 )
    {
      lable = 1;
    }
  else
    {
      lable = 0;
    }
  cout << "label: " << lable << endl;

  Mat ( * tocall ) ( Mat );

  if ( feature_type.compare ( "brief" ) == 0 )
    {
      tocall = is_person_present_brief;
    }
  else if ( feature_type.compare ( "brisk" ) == 0 )
    {
      tocall = is_person_present_brisk;
    }
  else if ( feature_type.compare ( "freak" ) == 0 )
    {
      tocall = is_person_present_freak;
    }
  else if ( feature_type.compare ( "hog" ) == 0 )
    {
      tocall = is_person_present_hog;
    }
  else if ( feature_type.compare ( "sift" ) == 0 )
    {
      tocall = is_person_present_sift;
    }
  else if ( feature_type.compare ( "surf" ) == 0 )
    {
      tocall = is_person_present_surf;
    }
  else if ( feature_type.compare ( "all" ) == 0 )
    {
      tocall = is_person_present_all;
    }
  else
    {
      return;
    }

  for ( int i = 0; i < nimages; i++ )
    {
      string stri = StringUtils::inttostr ( i, 4 );
      cout << "[" << stri << "] reading image: " << images_filenames[i] << endl;

      // reading the image in grey scale format
      Mat image = imread ( images_filenames[i], CV_LOAD_IMAGE_GRAYSCALE );
      if ( image.empty ( ) )
        {
          cerr << "Error! Couldn't read " << images_filenames[i] << endl;
          exit ( 1 );
        }

      Mat descriptor = tocall ( image );

      // writing the descriptor in the file
//       {
//         string descriptor_filename;
//         descriptor_filename.append ( output_folder );
//         descriptor_filename.append ( "/" );
//         descriptor_filename.append ( type );
//         descriptor_filename.append ( "/descriptor_" );
//         descriptor_filename.append ( feature_type );
//         descriptor_filename.append ( stri );
//         descriptor_filename.append ( ".yaml" );
//         FileStorage fs ( descriptor_filename, FileStorage::WRITE );
//         fs << "descriptor" << descriptor;
//         fs.release ( );
//       }

      // adding it to the training data set
      descriptors.push_back ( descriptor );

      // putting the lable
      labels.push_back ( lable ); // 1 = POSITIVE; 0 = NEGATIVE;
    }
}

Mat real_all_descriptors ( int argc, char** argv )
{
  vector< Mat > all_descriptors;
  int ncolumns = 0;
  for ( int i = 5; i < argc; i++ )
    {
      FileStorage fs;
      fs.open ( argv[i], FileStorage::READ );
      Mat tmpdescriptors;
      fs["descriptors"] >> tmpdescriptors;
      ncolumns += tmpdescriptors.cols;
      all_descriptors.push_back ( tmpdescriptors );
      fs.release();
      cout << "read image " << argv[i] << " size " << all_descriptors.back().rows << " x " << all_descriptors.back().cols << endl;
    }

  cout << "ncolumns " << ncolumns << endl;
  Mat descriptors ( atoi ( argv[4] ) * 2, ncolumns, CV_32FC1 );
  int copycolumn = 0;
  for ( unsigned int i = 0; i < all_descriptors.size(); i++ )
    {
      cout << "copycolumn " << copycolumn << endl;
      all_descriptors[i].copyTo ( Mat ( descriptors, Rect ( copycolumn, 0, all_descriptors[i].cols, all_descriptors[i].rows ) ) );
      copycolumn += all_descriptors[i].cols;
    }
  return descriptors;
}

int main ( int argc, char** argv )
{
  feature_type = argv[1];
  if ( feature_type.compare ( "brief" ) == 0 )
    {
      descriptor_length = brief_extractor.descriptorSize();
    }
  else if ( feature_type.compare ( "brisk" ) == 0 )
    {
      descriptor_length = brisk_extractor.descriptorSize();
    }
  else if ( feature_type.compare ( "freak" ) == 0 )
    {
      descriptor_length = freak_extractor.descriptorSize();
    }
  else if ( feature_type.compare ( "hog" ) == 0 )
    {
      descriptor_length = hog_extractor.getDescriptorSize();
    }
  else if ( feature_type.compare ( "sift" ) == 0 )
    {
      descriptor_length = sift_extractor.descriptorSize();
    }
  else if ( feature_type.compare ( "surf" ) == 0 )
    {
      descriptor_length = surf_extractor.descriptorSize();
    }
  else if ( feature_type.compare ( "all" ) == 0 )
    {
      descriptor_length = brief_extractor.descriptorSize() + brisk_extractor.descriptorSize() + freak_extractor.descriptorSize() + hog_extractor.getDescriptorSize() + sift_extractor.descriptorSize() + surf_extractor.descriptorSize();
    }

  cout << "descriptor_length: " << descriptor_length << endl;

  Mat descriptors ( 0, descriptor_length, CV_32FC1 );
  Mat labels ( 0, 1, CV_32FC1 );

  string output_folder = argv[3];

  /// combination of descriptors
  descriptors = real_all_descriptors ( argc, argv );
  cout << "descriptors " << descriptors.rows << " " << descriptors.cols << endl;
  int numimage = atoi ( argv[4] );
  for ( int i = 0; i < numimage; i++ )
    {
      labels.push_back ( 1 );
    }
  for ( int i = 0; i < numimage; i++ )
    {
      labels.push_back ( 0 );
    }
  cout << "labels " << labels.rows << " " << labels.cols << endl;

  /// using previous existing file
//   FileStorage fs;
//   string mat_filename = argv[5];
//   cout << "reading file " << mat_filename << endl;
//   fs.open ( mat_filename, FileStorage::READ );
//   if ( !fs.isOpened() )
//     {
//       cerr << "Failed to open " << mat_filename << endl;
//       return 1;
//     }
//   fs["descriptors"] >> descriptors;
//   cout << "descriptors " << descriptors.rows << " " << descriptors.cols << endl;
//   fs.release();
//
//   int numimage = atoi ( argv[4] );
//   for ( int i = 0; i < numimage; i++ )
//     {
//       labels.push_back ( 1 );
//     }
//   for ( int i = 0; i < numimage; i++ )
//     {
//       labels.push_back ( 0 );
//     }
//   cout << "labels " << labels.rows << " " << labels.cols << endl;

  /// extracting features
//   extract_features ( labels, descriptors, argv[4], output_folder, "pos" );
//   extract_features ( labels, descriptors, argv[5], output_folder, "neg" );
//
//   // write the descriptors to file so we don't need to do it all the time
//   {
//     string descriptors_filename;
//     descriptors_filename.append ( output_folder );
//     descriptors_filename.append ( "descriptors_" );
//     descriptors_filename.append ( feature_type );
//     descriptors_filename.append ( ".yaml" );
//     FileStorage fs ( descriptors_filename, FileStorage::WRITE );
//     fs << "descriptors" << descriptors;
//     fs.release ( );
//   }

  /// ml
  string ml_type;

  // train
  ml_type = argv[2];

  /// svm
  CvSVMParams svm_params;
  svm_params.svm_type    = SVM::C_SVC;
  svm_params.C           = 0.1;
  svm_params.nu          = 0.1;
  svm_params.kernel_type = SVM::LINEAR;
  svm_params.term_crit   = TermCriteria ( CV_TERMCRIT_ITER, ( int ) 1e7, 1e-6 );

  /// knn
//   CvKNearest knn;
//   ml_type = "knn";

  /// ann_mlp
  CvANN_MLP_TrainParams ann_mlp_params;
  ann_mlp_params.term_crit    = TermCriteria ( CV_TERMCRIT_ITER, ( int ) 1e7, 1e-6 );
  ann_mlp_params.train_method = CvANN_MLP_TrainParams::RPROP;

  /// em

  /// r_trees
  int r_trees_min_sample_count               = descriptors.rows * 0.01;
  int r_trees_max_depth                      = 25;
  int r_trees_params_categories              = 10;
  int r_trees_max_num_of_trees_in_the_forest = 100;
  float r_trees_forest_accuracy              = 0.01;
  int r_trees_termcrit_type                  = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;
  CvRTParams r_trees_params ( r_trees_max_depth, r_trees_min_sample_count, 0.0, false, r_trees_params_categories, 0, false, 0, r_trees_max_num_of_trees_in_the_forest, r_trees_forest_accuracy, r_trees_termcrit_type );

  /// er_trees
  CvRTParams er_trees_params ( r_trees_max_depth, r_trees_min_sample_count, 0.0, false, r_trees_params_categories, 0, false, 0, r_trees_max_num_of_trees_in_the_forest, r_trees_forest_accuracy, r_trees_termcrit_type );

  /// gb_trees
  int gb_trees_params_max_depth    = 3;
  int gb_trees_params_weak_count   = 600;
  float gb_trees_shrinkage         = 0.01;
  float gb_trees_subsample_portion = 0.8;
  CvGBTreesParams gb_trees_params ( CvGBTrees::DEVIANCE_LOSS, gb_trees_params_weak_count, gb_trees_shrinkage, gb_trees_subsample_portion, gb_trees_params_max_depth, false );

  /// boost
  int boost_weak_count          = 600;
  double boost_weight_trim_rate = 0.99;
  int boost_max_depth           = 1;
  CvBoostParams boost_params ( CvBoost::REAL, boost_weak_count, boost_weight_trim_rate, boost_max_depth, true, 0 );
  boost_params.min_sample_count = descriptors.rows * 0.01;

  /// d_trees
  CvDTreeParams d_trees_params;
  d_trees_params.min_sample_count = descriptors.rows * 0.01;
  d_trees_params.max_depth        = 25;
  d_trees_params.cv_folds         = 10;
  d_trees_params.max_categories   = 10;

  /// nbc

  // write responsed to file
  string ml_filename;
  ml_filename.append ( output_folder );
  ml_filename.append ( "ml_" );
  ml_filename.append ( ml_type );
  ml_filename.append ( feature_type );
  ml_filename.append ( ".yaml" );

  cout << "Starting to train ml_type: " << ml_type << endl;
  time_t start;
  time_t end;
  time ( &start );
  if ( ml_type.compare ( "svm" ) == 0 )
    {
      //   svm.train_auto ( descriptors, labels, Mat(), Mat(), svm_params, 10 );
      svm.train ( descriptors, labels, Mat(), Mat(), svm_params );
      svm.save ( ml_filename.c_str ( ) );
    }
  else if ( ml_type.compare ( "knn" ) == 0 )
    {
      knn.train ( descriptors, labels );
      knn.save ( ml_filename.c_str ( ) );
    }
  else if ( ml_type.compare ( "ann_mlp" ) == 0 )
    {
      ann_mlp.train ( descriptors, labels, Mat(), Mat(), ann_mlp_params );
      ann_mlp.save ( ml_filename.c_str ( ) );
    }
  else if ( ml_type.compare ( "em" ) == 0 )
    {
      em.train ( descriptors );
      // FIXME: can you save it?
//       em.write ( ml_filename.c_str ( ) );
    }
  else if ( ml_type.compare ( "er_trees" ) == 0 )
    {
      er_trees.train ( descriptors, CV_ROW_SAMPLE, labels, Mat(), Mat(), Mat(), Mat(), er_trees_params );
      er_trees.save ( ml_filename.c_str ( ) );
    }
  else if ( ml_type.compare ( "r_trees" ) == 0 )
    {
      r_trees.train ( descriptors, CV_ROW_SAMPLE, labels, Mat(), Mat(), Mat(), Mat(), r_trees_params );
      r_trees.save ( ml_filename.c_str ( ) );
    }
  else if ( ml_type.compare ( "gb_trees" ) == 0 )
    {
      gb_trees.train ( descriptors, CV_ROW_SAMPLE, labels, Mat(), Mat(), Mat(), Mat(), gb_trees_params );
      gb_trees.save ( ml_filename.c_str ( ) );
    }
  else if ( ml_type.compare ( "boost" ) == 0 )
    {
      boost.train ( descriptors, CV_ROW_SAMPLE, labels, Mat(), Mat(), Mat(), Mat(), boost_params );
      boost.save ( ml_filename.c_str ( ) );
    }
  else if ( ml_type.compare ( "d_trees" ) == 0 )
    {
      d_trees.train ( descriptors, CV_ROW_SAMPLE, labels, Mat(), Mat(), Mat(), Mat(), d_trees_params );
      d_trees.save ( ml_filename.c_str ( ) );
    }
  else if ( ml_type.compare ( "nbc" ) == 0 )
    {
      nbc.train ( descriptors, labels );
      nbc.save ( ml_filename.c_str ( ) );
    }

  time ( &end );
  cout << "Elapssed time is " << difftime ( end, start ) << endl;

  return 0;
}
