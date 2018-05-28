#include "utils/GtUtils.h"

#include "../../src/utils/opencv/CvUtils.h"
#include "../../src/utils/FileUtils.h"

#include <iostream>
#include <functional>
#include <numeric>

using namespace cv;
using namespace std;

int main ( int argc, char** argv )
{
  string outputfile = argv[1];
  cout << "argc: " << argc << endl;
  vector< double > tprs;
  vector< double > tnrs;
  vector< double > fprs;
  vector< double > fnrs;
  vector< double > accuracies;
  vector< double > precisions;

  for ( int i = 2; i < argc; i++ )
    {
      cout << "reading gt file:" << argv[i] << endl;
      FileStorage fs ( argv[i], FileStorage::READ );
      tprs.push_back ( ( double ) fs["TPR"] );
      tnrs.push_back ( ( double ) fs["TNR"] );
      fprs.push_back ( ( double ) fs["FPR"] );
      fnrs.push_back ( ( double ) fs["FNR"] );
      accuracies.push_back ( ( double ) fs["Accuracy"] );
      precisions.push_back ( ( double ) fs["Precision"] );
      fs.release();
    }

  FileStorage fsc ( outputfile, FileStorage::WRITE );

  double sum_tprs = accumulate ( tprs.begin(), tprs.end(), 0.0 );
  double mean_tprs = sum_tprs / ( double ) tprs.size();
  fsc << "TprsMean" << mean_tprs;

  double sum_tnrs = accumulate ( tnrs.begin(), tnrs.end(), 0.0 );
  double mean_tnrs = sum_tnrs / ( double ) tnrs.size();
  fsc << "TnrsMean" << mean_tnrs;

  double sum_fprs = accumulate ( fprs.begin(), fprs.end(), 0.0 );
  double mean_fprs = sum_fprs / ( double ) fprs.size();
  fsc << "FprsMean" << mean_fprs;

  double sum_fnrs = accumulate ( fnrs.begin(), fnrs.end(), 0.0 );
  double mean_fnrs = sum_fnrs / ( double ) fnrs.size();
  fsc << "FnrsMean" << mean_fnrs;

  double sum_accuracy = accumulate ( accuracies.begin(), accuracies.end(), 0.0 );
  double mean_accuracy = sum_accuracy / ( double ) accuracies.size();
  fsc << "AccuracyMean" << mean_accuracy;

  double sum_precision = accumulate ( precisions.begin(), precisions.end(), 0.0 );
  double mean_precision = sum_precision / ( double ) precisions.size();
  fsc << "PrecisionMean" << mean_precision;

  fsc.release();

  return 0;
}
