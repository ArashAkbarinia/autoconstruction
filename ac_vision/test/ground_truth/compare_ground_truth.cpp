#include "utils/GtUtils.h"

#include "../../src/utils/opencv/CvUtils.h"
#include "../../src/utils/FileUtils.h"

using namespace cv;
using namespace std;

int main ( int argc, char** argv )
{
  CvUtils cv_utils;

  vector< vector< Point > > gts;
  vector< string > filenames;
  cout << "argc: " << argc << endl;
  for ( int i = 3; i < argc; i++ )
    {
      cout << "reading gt file:" << argv[i] << endl;
      filenames.push_back ( argv[i] );
    }
  GtUtils::create_combine_gts_vector ( filenames, gts );
  cout << "Filenames: " << filenames.size() << endl;
  cout << "gts: " << gts.size() << " 2nd size: " << gts[0].size() << endl;
  vector< vector< Point > > results;
  string results_filename = argv[1];
  cout << "reading results file:" << results_filename << endl;
  FileUtils::read_results_from_file ( results_filename, results );
  cout << "results: " << results.size() << " 2nd size: " << results[0].size() << endl;

  GtUtils::compare_gts_results ( gts, results, argv[2] );

  return 0;
}
