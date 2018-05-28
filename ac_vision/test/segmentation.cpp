#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "../src/utils/FileUtils.h"

using namespace cv;
using namespace std;

/// Global variables

Mat src, src_hsv;

// hue
int low_threshold_hue = 25;
int high_threshold_hue = 60;
int low_threshold_hue2 = 25;
int high_threshold_hue2 = 60;
int max_low_hue = 180;
// saturation
int low_threshold_saturation = 50;
int high_threshold_saturation = 255;
int low_threshold_saturation2 = 150;
int high_threshold_saturation2 = 255;
int max_low_saturation = 255;
// value
int low_threshold_value = 205;
int high_threshold_value = 255;
int low_threshold_value2 = 90;
int high_threshold_value2 = 255;
int max_low_value = 255;
// gaussian
int gaussian_size = 3; // (3 x 2) + 1
string window_name = "Trackers";

void do_segmentation ( int, void* )
{
  Mat bw1;
  inRange ( src_hsv, Scalar ( low_threshold_hue, low_threshold_saturation, low_threshold_value ), Scalar ( high_threshold_hue, high_threshold_saturation, high_threshold_value ), bw1 );
  Mat bw2;
  inRange ( src_hsv, Scalar ( low_threshold_hue2, low_threshold_saturation2, low_threshold_value2 ), Scalar ( high_threshold_hue2, high_threshold_saturation2, high_threshold_value2 ), bw2 );
  Mat bw;
  bw = bw1 | bw2;

  imshow ( "BW1", bw1 );
  imshow ( "BW2", bw2 );
  imshow ( "BW", bw );
  imshow ( window_name, bw );
}

void do_gaussianblur ( int, void* )
{
  /// Convert the image to hsv
  cvtColor ( src, src_hsv, CV_BGR2HSV );
  GaussianBlur ( src_hsv, src_hsv, Size ( gaussian_size * 2 + 1, gaussian_size * 2 + 1 ), 0 );
  do_segmentation ( 0, 0 );
}

int main ( int argc, char** argv )
{
  vector< string > images_filenames;
  FileUtils::read_directory ( argv[1], images_filenames );

  if ( images_filenames.size() == 0 )
    {
      return -1;
    }

  /// Create a window
  namedWindow ( window_name, CV_WINDOW_NORMAL );

  /// Create a Trackbar for user to enter threshold
  createTrackbar ( "Min Hue:", window_name, &low_threshold_hue, max_low_hue, do_segmentation );
  createTrackbar ( "Min Saturation:", window_name, &low_threshold_saturation, max_low_saturation, do_segmentation );
  createTrackbar ( "Min Value:", window_name, &low_threshold_value, max_low_value, do_segmentation );
  createTrackbar ( "Max Hue:", window_name, &high_threshold_hue, max_low_hue, do_segmentation );
  createTrackbar ( "Max Saturation:", window_name, &high_threshold_saturation, max_low_saturation, do_segmentation );
  createTrackbar ( "Max Value:", window_name, &high_threshold_value, max_low_value, do_segmentation );

  createTrackbar ( "Min Hue2:", window_name, &low_threshold_hue2, max_low_hue, do_segmentation );
  createTrackbar ( "Min Saturation2:", window_name, &low_threshold_saturation2, max_low_saturation, do_segmentation );
  createTrackbar ( "Min Value2:", window_name, &low_threshold_value2, max_low_value, do_segmentation );
  createTrackbar ( "Max Hue2:", window_name, &high_threshold_hue2, max_low_hue, do_segmentation );
  createTrackbar ( "Max Saturation2:", window_name, &high_threshold_saturation2, max_low_saturation, do_segmentation );
  createTrackbar ( "Max Value2:", window_name, &high_threshold_value2, max_low_value, do_segmentation );

  createTrackbar ( "Gaussian:", window_name, &gaussian_size, 15, do_gaussianblur );

  for ( unsigned int i = 0; i < images_filenames.size(); i++ )
    {
      src = imread ( images_filenames[i] );
      if ( src.empty() )  continue;
      if ( src.rows > 600 )
        {
          resize ( src, src, Size(), 0.5, 0.5, INTER_CUBIC );
        }
      while ( true )
        {
          /// Show the image
          imshow ( "original", src );
          do_gaussianblur ( 0, 0 );

          /// Wait until user exit program by pressing a key

          char c = waitKey ( 3 );
          if ( c == 'n' )
            {
              break;
            }
        }
    }

  return 0;
}