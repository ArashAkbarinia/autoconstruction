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

#include <ros/ros.h>
#include <ros/package.h>
#include <std_srvs/Empty.h>
#include <time.h>
#include <sys/time.h>
#include <functional>
#include <numeric>

#include "ac_vision_constants.h"
#include "tracking/HumanTracker.h"
#include "utils/ApplicationSession.h"
#include "utils/ApplicationSettings.h"
#include "utils/FileUtils.h"
#include "utils/StringUtils.h"
#include "utils/opencv/CvUtils.h"
#include "utils/opencv/ForegroundExtractor.h"
#include "utils/opencv/HumanDetector.h"
#include "utils/opencv/SafetyJacketDetector.h"

using namespace std;
using namespace cv;

bool object_selected = false;
bool is_selection_done = false;
Point origin;
Rect selection;
Mat frame;

ForegroundExtractor foreground_extractor;
// HumanDetector human_detector;
HumanTracker human_tracker;
SafetyJacketDetector safety_jacket_detector;
CvUtils cv_utils;

long long getmsofday ( void )
{
  struct timeval tv;
  gettimeofday ( &tv, 0 );
  return ( long long ) tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void mouse_callback ( int event, int x, int y, int, void* userdata )
{
  if ( object_selected )
    {
      selection.x = MIN ( x, origin.x );
      selection.y = MIN ( y, origin.y );
      selection.width = abs ( x - origin.x );
      selection.height = abs ( y - origin.y );
      selection &= Rect ( 0, 0, frame.cols, frame.rows );
    }

  switch ( event )
    {
    case CV_EVENT_LBUTTONDOWN:
      origin = Point ( x, y );
      selection = Rect ( x, y, 0, 0 );
      object_selected = true;
      break;
    case CV_EVENT_LBUTTONUP:
      object_selected = false;
      if ( selection.width > 0 && selection.height > 0 )
        {
          is_selection_done = true;
        }
      break;
    }
}

void select_object ( void )
{
  while ( !is_selection_done )
    {
      Mat image;
      frame.copyTo ( image );
      if ( selection.width > 0 && selection.height > 0 )
        {
          Mat roi ( image, selection );
          bitwise_not ( roi, roi );
        }
      imshow ( AC_VISION_NODE, image );
      waitKey ( 5 );
    }
  cout << "Target was manually selected: " << selection << endl;
  is_selection_done = false;
}

void handle_user_input ( char input )
{
  switch ( input )
    {
    case ' ':
      select_object ( );
      break;
    default:
      break;
    }
}

void do_background_subtraction ( void )
{
  Mat foreground_gmg = foreground_extractor.get_foreground_gmg ( frame );
  imshow ( "Foreground GMG", foreground_gmg );
  Mat foreground_mog = foreground_extractor.get_foreground_mog ( frame );
  imshow ( "Foreground MOG", foreground_mog );
  Mat foreground_mog2 = foreground_extractor.get_foreground_mog2 ( frame );
  imshow ( "Foreground MOG2", foreground_mog2 );
  waitKey ( 3 );
}

void do_detection ( void )
{
//   vector< Rect > cascade_humans_full = human_detector.get_cascade_detected_humans_full_body ( frame );
//   vector< Rect > cascade_humans_lower = human_detector.get_cascade_detected_humans_lower_body ( frame );
//   vector< Rect > cascade_humans_upper = human_detector.get_cascade_detected_humans_upper_body ( frame );
//   vector< Rect > cascade_pedestrains = human_detector.get_cascade_detected_pedestrains ( frame );
//   vector< Rect > hog_humans = human_detector.get_hog_detected_humans ( frame );
//   vector< Rect > latent_humans = human_detector.get_latent_svm_detected_humans ( frame );

//   cv_utils.draw_detected_humans ( frame, cascade_humans_full, WHITE );
//   cv_utils.draw_detected_humans ( frame, cascade_humans_lower, RED );
//   cv_utils.draw_detected_humans ( frame, cascade_humans_upper, BLUE );
//   cv_utils.draw_detected_humans ( frame, cascade_pedestrains, YELLOW );
//   cv_utils.draw_detected_humans ( frame, hog_humans, GREEN );
//   cv_utils.draw_detected_humans ( frame, latent_humans, BLACK );

  imshow ( AC_VISION_NODE, frame );
}

vector< Rect > do_tracking ( vector< SafetyJacket > jackets )
{
  vector< Rect > results = human_tracker.track ( frame, jackets );

  return results;
}

void do_segmentation ( void )
{
  Mat hsv;
  cvtColor ( frame, hsv, CV_BGR2HSV );
  GaussianBlur ( hsv, hsv, SAFETY_JACKET_GAUSSIAN_SIZE, 0 );

  Mat bw1;
  inRange ( hsv, SAFETY_JACKET_COLOUR_LOW1, SAFETY_JACKET_COLOUR_HIGH1, bw1 );
  Mat bw2;
  inRange ( hsv, SAFETY_JACKET_COLOUR_LOW2, SAFETY_JACKET_COLOUR_HIGH2, bw2 );
  Mat binarised_image;
  binarised_image = bw1 | bw2;

  imshow ( "segmentation", binarised_image );

  morphologyEx ( binarised_image, binarised_image, MORPH_CLOSE, SAFETY_JACKET_CLOSE_ELEMENT );
  morphologyEx ( binarised_image, binarised_image, MORPH_OPEN, SAFETY_JACKET_OPEN_ELEMENT );

  imshow ( "morph", binarised_image );

  imshow ( AC_VISION_NODE, frame );
  waitKey ( 0 );
}

vector< SafetyJacket > do_safetyjacket_detection ( void )
{
  vector< SafetyJacket > jackets;
  safety_jacket_detector.detect ( frame, jackets );

  return jackets;
}

void save_rois ( vector< Rect > frame_results )
{
  for ( unsigned int i = 0; i < frame_results.size(); i++ )
    {
      Mat roi ( frame, frame_results[i] );
      string roi_filename = "resources/test_data/frames/pos/test";
      roi_filename.append ( StringUtils::inttostr ( ApplicationSettings::counter, 5 ) );
      ApplicationSettings::counter++;
      roi_filename.append ( ".png" );
      imwrite ( roi_filename, roi );
    }
}

int main ( int argc, char** argv )
{
  ros::init ( argc, argv, AC_VISION_NODE );

  ApplicationSession application_session ( argc, argv );
  HumanDetector::initialise ( );
  human_tracker.initialise (  );

  // gui
  namedWindow ( AC_VISION_NODE, WINDOW_AUTOSIZE );
  setMouseCallback ( AC_VISION_NODE, mouse_callback, 0 );

  vector< vector< Rect > > results;
  vector< long long > undistoring_time;
  vector< long long > jackets_time;
  vector< long long > tracking_time;

  while ( true )
    {
      frame = application_session.read_one_frame ( );
      if ( frame.empty ( ) )
        {
          break;
        }
      LOG_DEBUG ( "MAIN: processing frame " << application_session.get_num_frames() );
//       if ( application_session.get_num_frames() % 17 != 0 )
//         {
//           continue;
//         }

      long long start = getmsofday();
      frame = cv_utils.correct_progo_image ( frame );
      long long end = getmsofday();
      undistoring_time.push_back( end - start );

      // saving the frame
//       string filename = "positive";
//       filename.append ( StringUtils::inttostr ( counter, 4 ) );
//       counter++;
//       filename.append ( ".png" );
//       imwrite ( filename, frame );

      // background subtraction
//       do_background_subtraction ( );

      // detections
//       do_detection ( );

      // segmenting
//       do_segmentation ( );

      // safety jacket detection
      long long start1 = getmsofday();
      vector< SafetyJacket > jackets = do_safetyjacket_detection ( );
      long long end1 = getmsofday();
      jackets_time.push_back ( end1 - start1 );
//       vector< Rect > humans;
//       long long start2 = getmsofday();
//       for ( unsigned int i = 0; i < jackets.size(); i++ )
//         {
//           if ( HumanDetector::is_person_present ( Mat ( frame, jackets[i].human ), ML_SIFT_SVM ) &&
//                HumanDetector::is_person_present ( Mat ( frame, jackets[i].human ), ML_LATENT_SVM ) )
//             {
//               humans.push_back ( jackets[i].human );
//             }
//         }
//       long long end2 = getmsofday();
//       tracking_time.push_back ( end2 - start2 );

      // tracking
      long long start2 = getmsofday();
      vector< Rect > humans = do_tracking ( jackets );
      long long end2 = getmsofday();
      tracking_time.push_back ( end2 - start2 );

//       save_rois ( frame_results );

      results.push_back ( humans );

      cv_utils.draw_detected_humans ( frame, humans, RED );
      application_session.add_frame_to_video_writer ( frame );
//       imshow ( AC_VISION_NODE, frame );
//       waitKey ( ApplicationSettings::waiting_time );
    }

  double sum_undistorting = accumulate ( undistoring_time.begin(), undistoring_time.end(), 0.0 );
  double mean_undistorting = sum_undistorting / ( double ) undistoring_time.size();
  cout << "Undistorting time: " << mean_undistorting << endl;
  double sum_jackets = accumulate ( jackets_time.begin(), jackets_time.end(), 0.0 );
  double mean_jackets = sum_jackets / ( double ) jackets_time.size();
  cout << "Jackets time: " << mean_jackets << endl;
  double sum_tracking = accumulate ( tracking_time.begin(), tracking_time.end(), 0.0 );
  double mean_tracking = sum_tracking / ( double ) tracking_time.size();
  cout << "Tracking time: " << mean_tracking<< endl;
  FileUtils::write_results_to_file ( application_session.get_output_file(), results );
  cout << "Num frames: " << application_session.get_num_frames ( ) << endl;

  return 0;
}