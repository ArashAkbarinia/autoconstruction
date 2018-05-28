#include "../src/utils/FileUtils.h"
#include "../src/utils/StringUtils.h"

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main ( int argc, char** argv )
{
  vector< string > images_filenames;
  FileUtils::read_directory ( argv[1], images_filenames );
  bool rename = false;
  string suffix = "";
  if ( argc >= 3 )
    {
      rename = true;
      suffix = argv[2];
    }

  if ( images_filenames.size() == 0 )
    {
      return -1;
    }

  for ( unsigned int i = 0; i < images_filenames.size ( ); i++ )
    {
      Mat src = imread ( images_filenames[i] );
      if ( src.empty ( ) )
        {
          cerr << "Skipped: " << images_filenames[i] << endl;
          continue;
        }
      if ( src.rows != src.cols )
        {
          cerr << "Not square: " << images_filenames[i] << endl;
          continue;
        }
      if ( src.rows == 64 )
        {
          continue;
        }

      resize ( src, src, Size ( 64, 64 ), 0, 0, INTER_CUBIC );
      string output_filename = images_filenames[i];
      if ( rename )
        {
          output_filename = argv[1];
          output_filename.append ( suffix );
          output_filename.append ( StringUtils::inttostr ( i, 5 ) );
          output_filename.append ( ".png" );
        }
      imwrite ( output_filename, src );
      cout << "Done: " << images_filenames[i] << endl;
    }

  return 0;
}