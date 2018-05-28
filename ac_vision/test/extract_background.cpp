#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

int main ( int argc, char** argv )
{
  /// Load an image
  Mat image = imread ( argv[1] );

  if ( !image.data )
    {
      return -1;
    }

  int width = atoi ( argv[2] );
  int height = atoi ( argv[3] );
  int counter = atoi ( argv[4] );
  cout << "Width " << width << " Height " << height << " Counter " << counter << endl;
  for ( int y = 0; y <= image.rows - height; y = y + height )
    {
      for ( int x = 0; x <= image.cols - width; x = x + width )
        {
          counter++;
          Rect cur_rect ( x, y, width, height );
          Mat cur_win ( image, cur_rect );
          string filename = "negative";
          if ( counter < 10 )
            {
              filename.append ( "000" );
            }
          else if ( counter < 100 )
            {
              filename.append ( "00" );
            }
          else if ( counter < 1000 )
            {
              filename.append ( "0" );
            }
          stringstream ss;
          ss << counter;
          filename.append ( ss.str() );
          filename.append ( ".jpg" );
          cout << "writing " << filename << endl;
          imwrite ( filename, cur_win );
        }
    }

  return 0;
}