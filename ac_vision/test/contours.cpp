#include "../src/utils/FileUtils.h"
#include "../src/ac_vision_constants.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/// Global variables
Mat src;
int contourtype = CV_RETR_TREE;
int method = CV_CHAIN_APPROX_SIMPLE;

void draw_contours ( int, void* )
{
  Mat hsv;
  cvtColor ( src, hsv, CV_BGR2HSV );
  GaussianBlur ( hsv, hsv, SAFETY_JACKET_GAUSSIAN_SIZE, 0 );

  Mat bw1;
  inRange ( hsv, SAFETY_JACKET_COLOUR_LOW1, SAFETY_JACKET_COLOUR_HIGH1, bw1 );
  Mat bw2;
  inRange ( hsv, SAFETY_JACKET_COLOUR_LOW2, SAFETY_JACKET_COLOUR_HIGH2, bw2 );
  Mat binarised_image;
  binarised_image = bw1 | bw2;

  morphologyEx ( binarised_image, binarised_image, MORPH_CLOSE, SAFETY_JACKET_CLOSE_ELEMENT );
  morphologyEx ( binarised_image, binarised_image, MORPH_OPEN, SAFETY_JACKET_OPEN_ELEMENT );

  imshow ( "Binary", binarised_image );
  imshow ( "Original", src );
  waitKey ( 3 );

  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  /// Find contours
  findContours ( binarised_image, contours, hierarchy, contourtype, method, Point ( 0, 0 ) );

  /// Approximate contours to polygons + get bounding rects and circles
  vector<vector<Point> > contours_poly ( contours.size() );
  vector<Rect> boundRect ( contours.size() );
  vector<Point2f>center ( contours.size() );
  vector<float>radius ( contours.size() );

  for ( size_t i = 0; i < contours.size(); i++ )
    {
      approxPolyDP ( Mat ( contours[i] ), contours_poly[i], 3, true );
      boundRect[i] = boundingRect ( Mat ( contours_poly[i] ) );
      minEnclosingCircle ( contours_poly[i], center[i], radius[i] );
    }

  /// Draw polygonal contour + bonding rects + circles
  Mat drawing = Mat::zeros ( binarised_image.size(), CV_8UC3 );
  for ( size_t i = 0; i< contours.size(); i++ )
    {
      Scalar color = Scalar ( 255, 255, 255 );
      drawContours ( drawing, contours_poly, ( int ) i, color, 1, 8, vector<Vec4i>(), 0, Point() );
      rectangle ( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
      circle ( drawing, center[i], ( int ) radius[i], color, 2, 8, 0 );
    }

  /// Show in a window
  imshow ( "Contours", drawing );
}

int main ( int argc, char** argv )
{
  vector< string > images_filenames;
  FileUtils::read_directory ( argv[1], images_filenames );

  if ( images_filenames.size() == 0 )
    {
      return -1;
    }

  namedWindow ( "Original", CV_WINDOW_AUTOSIZE );
  createTrackbar ( "Mode: ", "Original", &contourtype, 4, draw_contours );
  createTrackbar ( "Method: ", "Original", &method, 5, draw_contours );

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
      draw_contours ( 0, 0 );

      while ( true )
        {
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