#include "../../src/utils/ApplicationSession.h"
#include "../../src/utils/FileUtils.h"
#include "../../src/utils/opencv/CvUtils.h"

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

using namespace cv;
using namespace std;

Mat image;

int track_object = 0;
int skip = 0;
bool select_object = false;
Point origin;
Rect selection;
vector< Rect > to_be_saved;
CvUtils cv_utils;

Rect check_boundaries ( Rect window )
{
  Rect bound ( 0, 0, image.cols, image.rows );
  Rect new_window = window & bound;

  return new_window;
}

void on_mouse ( int event, int x, int y, int, void* )
{
  if ( select_object && ( skip  == 0 ) )
    {
      selection.x = MIN ( x, origin.x );
      selection.y = MIN ( y, origin.y );
      selection.width = abs ( x - origin.x );
      selection.height = abs ( y - origin.y );
      selection = check_boundaries ( selection );
      selection = cv_utils.squareise_window ( selection, image );
    }

  switch ( event )
    {
    case CV_EVENT_LBUTTONDOWN:
      origin = Point ( x, y );
      selection = Rect ( x, y, 0, 0 );
      select_object = true;
      skip = 0;
      break;
    case CV_EVENT_LBUTTONUP:
      select_object = false;
      if ( selection.width > 0 && selection.height > 0 )
        {
          track_object = -1;
        }
      break;
    case CV_EVENT_RBUTTONDBLCLK:
      select_object = true;
      selection = Rect ( 0, 0, 0, 0 );
      track_object = -1;
      skip = 1;
      break;
    }
}

void help()
{
  cout << "\nThis program creates the ground-truth for a tracking algorithm.\n"
       "You select the face of one person in order to create the ground-truth.\n"
       "This reads from a video or text file containing the names of the frames.\n"
       "The program saves to .yaml file  the ground-truth.\n"
       "Usage: \n"
       "  ./create_ground_truth [input_video/.txt] [output file]\n";

  cout << "\n\nHot keys: \n"
       "\tup arrow - move up ROI\n"
       "\tdown arrow - move down ROI\n"
       "\tleft arrow - move left ROI\n"
       "\tright arrow - move right ROI\n"
       "\to - increase scale of ROI\n"
       "\ti - decrease scale of ROI\n"
       "\tc - skip ROI\n"
       "\tspace - goes to the next frame\n"
       "To initialise ground-truth, select the object with mouse\n";
}

int main ( int argc, char** argv )
{
  ApplicationSession application_session ( argc, argv );
  CvUtils cv_utils;
  Mat frame;
  Rect track_window;
  int go_next = 1;

  help();

  namedWindow ( "Input", WINDOW_AUTOSIZE );
  setMouseCallback ( "Input", on_mouse, 0 );

  while ( true )
    {
      if ( go_next == 1 )
        {
          cout << "reading the " << application_session.get_num_frames ( ) << " frame" << endl;
          frame = application_session.read_one_frame ( );
          if ( frame.empty ( ) )
            {
              break;
            }
          frame = cv_utils.correct_progo_image ( frame );
          go_next = 0;
        }

      frame.copyTo ( image );

      if ( select_object )
        {
          track_window = selection;
        }

      if ( select_object && selection.width > 0 && selection.height > 0 )
        {
          Mat roi ( image, selection );
          bitwise_not ( roi, roi );
        }

      track_window = check_boundaries ( track_window );
      track_window = cv_utils.squareise_window ( track_window, image );
      rectangle ( image, track_window, RED, 2, 8, 0 );
      Point centre = cv_utils.get_rect_centre ( track_window );
      circle ( image, centre, 1, RED, -1 );
      imshow ( "Input", image );

      int pressed_button = waitKey ( 3 );
      // escape finished the programme
      if ( pressed_button == 1048603 ) //27 
        {
          break;
        }

      switch ( pressed_button )
        {
        case 1113937://65361: // left arrow
          track_window.x = track_window.x - 1;
          break;
        case 1113938://65362: // up arrow
          track_window.y = track_window.y - 1;
          break;
        case 1113939://65363: // right arrow
          track_window.x = track_window.x + 1;
          break;
        case 1113940://65364: // down arrow
          track_window.y = track_window.y + 1;
          break;
        case 1048681://105: // i, zoom in
          track_window = cv_utils.enlarge_window ( track_window, image, 0.95 );
          break;
        case 1048687://111: // o, zoom out
          track_window = cv_utils.enlarge_window ( track_window, image, 1.05 );
          break;
        case 1048675://99: // c, cancel roi
          select_object = true;
          selection = Rect ( 0, 0, 0, 0 );
          track_object = -1;
          skip = 1;
        case 1048608://32: // space, move to next frame
          go_next = 1;
          to_be_saved.push_back ( track_window );
        default:
          break;
        }
    }

  // rrite rois to .yaml file
  FileUtils::write_rects_to_file ( application_session.get_output_file ( ), to_be_saved );

  return 0;
}
