#include "../../src/utils/opencv/CvUtils.h"
#include "../../src/utils/FileUtils.h"

int main ( int argc, char** argv )
{
  Mat image = Mat::zeros ( 360, 640, CV_32F );

  vector< Rect > gts;
  string input = argv[1];
  string output = argv[2];

  FileUtils::read_rects_from_file ( input, gts );

  CvUtils cv_utils;
  for ( unsigned int i = 0; i < gts.size(); i++ )
    {
      gts[i] = cv_utils.squareise_window ( gts[i], image );
    }

  FileUtils::write_rects_to_file ( output, gts );

  return 0;
}