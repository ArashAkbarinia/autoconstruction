#include "utils/GtUtils.h"

#include "../../src/utils/opencv/CvUtils.h"
#include "../../src/utils/StringUtils.h"

int main ( int argc, char** argv )
{
  VideoCapture video_capture ( argv[1] );
  video_capture.set ( CV_CAP_PROP_POS_MSEC, atoi ( argv[2] ) * 1000 );
  double end_second = atof ( argv[3] );
  int counter = atoi ( argv[4] );

  vector< vector< Rect > > gts;
  vector< string > filenames;

  for ( int i = 5; i < argc; i++ )
    {
      cout << "reading file:" << argv[i] << endl;
      filenames.push_back ( argv[i] );
    }
  GtUtils::create_combine_gts_vector ( filenames, gts );
  cout << "number of frames in ground truth: " << gts.size() << endl;

  float camera_matrix_vector[3][3] = { {469.967, 0, 640}, {0, 467.682, 360}, {0, 0, 1} };
  Mat camera_matrix ( 3, 3, CV_32FC1, camera_matrix_vector );
  float dist_coeffs_vector[5] = { -0.18957, 0.03732, 0, 0, -0.003367 };
  Mat dist_coeffs ( 1, 5, CV_32FC1, dist_coeffs_vector );

  Mat frame;

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
      Mat undistorted;
//       undistort ( frame, undistorted, camera_matrix, dist_coeffs );
      flip ( frame, frame, -1 );

      if ( frame.rows > 600 )
        {
          resize ( frame, frame, Size(), 0.5, 0.5, INTER_CUBIC );
        }

      for ( unsigned int i = 0; i < gts[nframe].size(); i++ )
        {
          Mat roi ( frame, gts[nframe][i] );
          string roi_filename = "testpos";
          roi_filename.append ( StringUtils::inttostr ( counter, 4 ) );
          counter++;
          roi_filename.append ( ".png" );
          imwrite ( roi_filename, roi );
        }
      nframe++;
    }

  return 0;
}