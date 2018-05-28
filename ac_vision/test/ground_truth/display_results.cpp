#include "utils/GtUtils.h"

#include "../../src/utils/opencv/CvUtils.h"

int main ( int argc, char** argv )
{
  VideoCapture video_capture ( argv[1] );
  video_capture.set ( CV_CAP_PROP_POS_MSEC, atoi ( argv[2] ) * 1000 );
  double end_second = atof ( argv[3] );
  int waitsec = atoi ( argv[4] );

  vector< vector< Rect > > gts;
  vector< string > filenames;

  for ( int i = 5; i < argc; i++ )
    {
      cout << "reading file:" << argv[i] << endl;
      filenames.push_back ( argv[i] );
    }
  GtUtils::create_combine_gts_vector ( filenames, gts );
  cout << "number of frames in ground truth: " << gts.size() << endl;

  CvUtils cv_utils;
  Mat frame;

  namedWindow ( "Input", WINDOW_AUTOSIZE );
  int nframe = 0;

  while ( true )
    {
      double cursecond = video_capture.get ( CV_CAP_PROP_POS_MSEC ) / 1000;
      if ( end_second != 0 && cursecond >= end_second )
        {
          break;
        }

      cout << "reading the " << nframe << " frame" << endl;
      video_capture >> frame;
      if ( frame.empty ( ) )
        {
          break;
        }
      frame = cv_utils.correct_progo_image ( frame );

      cv_utils.draw_detected_humans ( frame, gts[nframe] );
      imshow ( "Input", frame );
      waitKey ( waitsec );
      nframe++;
    }

  return 0;
}
