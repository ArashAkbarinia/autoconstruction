#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

#include "../src/utils/FileUtils.h"
#include "../src/ac_vision_constants.h"

using namespace cv;

/// Global variables

Mat src, src_hsv, bw;
int close_size = 7;
int open_size = 3;

string window_name = "Original";
string window_name_close = "Close";
string window_name_open = "Open";
string window_name_closeopen = "Close - Open";
string window_name_openclose = "Open - Close";

void do_morphology ( int, void* )
{
  Mat morph_close;
  Mat close_element = getStructuringElement ( MORPH_RECT, Size ( close_size, close_size ) );
  morphologyEx ( bw, morph_close, MORPH_CLOSE, close_element );
  Mat morph_open;
  Mat open_element = getStructuringElement ( MORPH_RECT, Size ( open_size, open_size ) );
  morphologyEx ( bw, morph_open, MORPH_OPEN, open_element );
  Mat morph_closeopen;
  morphologyEx ( morph_close, morph_closeopen, MORPH_OPEN, open_element );
  Mat morph_openclose;
  morphologyEx ( morph_open, morph_openclose, MORPH_CLOSE, close_element );

  imshow ( window_name_close, morph_close );
  imshow ( window_name_open, morph_open );
  imshow ( window_name_closeopen, morph_closeopen );
  imshow ( window_name_openclose, morph_openclose );
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
  namedWindow ( window_name_close, CV_WINDOW_NORMAL );
  namedWindow ( window_name_open, CV_WINDOW_NORMAL );
  namedWindow ( window_name_closeopen, CV_WINDOW_NORMAL );

  /// Create a Trackbar for user to enter threshold
  createTrackbar ( "Close: ", window_name, &close_size, 15, do_morphology );
  createTrackbar ( "Open: ", window_name, &open_size, 15, do_morphology );

  /// Wait until user exit program by pressing a key
  for ( unsigned int i = 0; i < images_filenames.size(); i++ )
    {
      src = imread ( images_filenames[i] );
      if ( src.empty() )  continue;
      if ( src.rows > 600 )
        {
          resize ( src, src, Size(), 0.5, 0.5, INTER_CUBIC );
        }

      /// Segment the image
      cvtColor ( src, src_hsv, CV_BGR2HSV );
      GaussianBlur ( src_hsv, src_hsv, SAFETY_JACKET_GAUSSIAN_SIZE, 0 );
      Mat segm1;
      inRange ( src_hsv, SAFETY_JACKET_COLOUR_LOW1, SAFETY_JACKET_COLOUR_HIGH1, segm1 );
      Mat segm2;
      inRange ( src_hsv, SAFETY_JACKET_COLOUR_LOW2, SAFETY_JACKET_COLOUR_HIGH2, segm2 );
      bw = segm1 | segm2;

      while ( true )
        {
          /// Show the image
          imshow ( window_name, src );
          imshow ( "segmented", bw );
          do_morphology ( 0, 0 );

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