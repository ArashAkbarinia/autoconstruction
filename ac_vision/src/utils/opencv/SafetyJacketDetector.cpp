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

#include "SafetyJacketDetector.h"

#include "../ApplicationSettings.h"
#include "../StringUtils.h"
#include "../../ac_vision_constants.h"

SafetyJacketDetector::SafetyJacketDetector ( void )
{

}

void SafetyJacketDetector::detect ( const Mat& image, vector< SafetyJacket >& jackets ) const
{
  jackets.clear();

  Mat binarised_image = binarise_image ( image );
  vector< SafetyJacket > segmented_jackets;
  find_jackets ( binarised_image, segmented_jackets );

#ifdef DEBUG_BLOB_DETECTOR
  Mat display_image1 = image.clone();
  for ( unsigned int i = 0; i < segmented_jackets.size(); i++ )
    {
      rectangle ( display_image1, segmented_jackets[i].jacket, RED );
      putText ( display_image1, StringUtils::inttostr ( segmented_jackets[i].jacket.height ), segmented_jackets[i].jacket.tl(), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, RED );
      putText ( display_image1, StringUtils::inttostr ( segmented_jackets[i].jacket.width ), segmented_jackets[i].jacket.br(), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, BLUE );
    }
  imshow ( "blobsnomerge", display_image1 );
  waitKey ( ApplicationSettings::waiting_time );
#endif

  vector< SafetyJacket > merged_jackets;
  merge_subblobs ( segmented_jackets, merged_jackets, ApplicationSettings::safety_jacket_intersection_percent1 );
  vector< SafetyJacket > removed_small_jackets;
  remove_smallblobs ( merged_jackets, removed_small_jackets, image.rows );
  cv_utils.get_upperbody_rects ( removed_small_jackets, image );
  merge_subblobs ( removed_small_jackets, jackets, ApplicationSettings::safety_jacket_intersection_percent2 );
  cv_utils.squareise_windows ( jackets, image );

#ifdef DEBUG_BLOB_DETECTOR
  Mat display_image2 = image.clone();
  cv_utils.draw_detected_humans ( display_image2, jackets, RED );
  imshow ( "blobsmerged", display_image2 );
#endif
}

void SafetyJacketDetector::find_jackets ( const Mat& binary_image, vector< SafetyJacket >& jackets ) const
{
  jackets.clear();

  vector< vector < Point > > contours;
  findContours ( binary_image.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );

  vector< vector< Point > > contours_poly ( contours.size() );
  jackets.resize ( contours.size() );

  for ( unsigned int i = 0; i < contours.size(); i++ )
    {
      approxPolyDP ( Mat ( contours[i] ), contours_poly[i], 3, true );
      jackets[i].jacket = boundingRect ( Mat ( contours_poly[i] ) );
      jackets[i].human = jackets[i].jacket;
    }
}

void SafetyJacketDetector::merge_subblobs ( const vector< SafetyJacket >& input, vector< SafetyJacket >& output, const double eps ) const
{
//   LOG_DEBUG ( "SafetyJacketDetector::merge_subblobs: size of all blobs " << input.size() );

  output.clear();

  // going through all the detected blobs if they're subset of each other keep the super only
  for ( unsigned int i = 0; i < input.size(); i++ )
    {
      // if too big just add it
      if ( input[i].human.area() > ApplicationSettings::safety_jacket_exception_area )
        {
          LOG_DEBUG ( "SafetyJacketDetector::merge_subblobs: exceptionally big " << input[i].human );
          output.push_back ( input[i] );
          continue;
        }

      bool is_new = true;
      Rect chuman = input[i].human;
      Rect cjacket = input[i].jacket;
      for ( unsigned int j = 0; j < output.size(); j++ )
        {
          if ( cv_utils.are_rects_subsets ( chuman, output[j].human ) )
            {
              is_new = false;
              if ( chuman.area() > output[j].human.area() )
                {
                  output[j].human = chuman;
                  output[j].jacket = cjacket;
                }
            }
          else
            {
              Rect intersection = chuman & output[j].human;
              if ( intersection.area() >= ( chuman.area() * eps ) ||
                   intersection.area() >= ( output[j].human.area() * eps ) )
                {
                  is_new = false;
                  output[j].human |= chuman;
                  output[j].jacket |= cjacket;
                  chuman = output[j].human;
                  cjacket = output[j].jacket;
                }
            }
        }
      if ( is_new )
        {
          output.push_back ( input[i] );
        }
    }

//   LOG_DEBUG ( "SafetyJacketDetector::merge_subblobs: size of super blobs " << output.size() );
}

void SafetyJacketDetector::remove_smallblobs ( const vector< SafetyJacket >& input, vector< SafetyJacket >& output, const float numrows ) const
{
//   LOG_DEBUG ( "SafetyJacketDetector::remove_smallblobs: size of input blobs " << input.size() );

  output.clear();

  float scale = ( ApplicationSettings::safety_jacket_max_area - ApplicationSettings::safety_jacket_min_area ) / numrows;
  for ( unsigned int i = 0; i < input.size(); i++ )
    {
      float minarea = ApplicationSettings::safety_jacket_min_area + ( scale * input[i].human.y );
      if ( input[i].human.area() > minarea )
        {
          output.push_back ( input[i] );
        }
      else
        {
//           LOG_DEBUG ( "SafetyJacketDetector::remove_smallblobs: minarea " << minarea << " actual area " << input[i].area() );
        }
    }

//   LOG_DEBUG ( "SafetyJacketDetector::remove_smallblobs: size of output blobs " << output.size() );
}

Mat SafetyJacketDetector::binarise_image ( const Mat& image ) const
{
  Mat hsv;
  cvtColor ( image, hsv, CV_BGR2HSV );
  GaussianBlur ( hsv, hsv, SAFETY_JACKET_GAUSSIAN_SIZE, 0 );

  Mat bw1;
  inRange ( hsv, SAFETY_JACKET_COLOUR_LOW1, SAFETY_JACKET_COLOUR_HIGH1, bw1 );
  Mat bw2;
  inRange ( hsv, SAFETY_JACKET_COLOUR_LOW2, SAFETY_JACKET_COLOUR_HIGH2, bw2 );
  Mat binarised_image;
  binarised_image = bw1 | bw2;
#ifdef DEBUG_BLOB_DETECTOR
  imshow ( "segmentation", binarised_image );
#endif
  morphologyEx ( binarised_image, binarised_image, MORPH_CLOSE, SAFETY_JACKET_CLOSE_ELEMENT );
  morphologyEx ( binarised_image, binarised_image, MORPH_OPEN, SAFETY_JACKET_OPEN_ELEMENT );
#ifdef DEBUG_BLOB_DETECTOR
  imshow ( "morph", binarised_image );
#endif
  return binarised_image;
}