#include "../../src/utils/FileUtils.h"
#include "../../src/utils/StringUtils.h"

#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

void read_from_file ( const string file_name, string type, vector< int >& labels )
{
  cout << "reading " << file_name << " " << type << endl;
  FileStorage fs ( file_name, FileStorage::READ );
  FileNode features = fs[type];
  FileNodeIterator itb = features.begin();
  FileNodeIterator ite = features.end();

  for ( ; itb != ite; ++itb )
    {
      int f = ( int ) ( *itb ) ["f"];
      labels.push_back ( f );
    }
  fs.release();
}

int main ( int argc, char** argv )
{
  string posfile = argv[1];
  string negfile = argv[2];

  vector< int > pos_results;
  vector< int > neg_results;
  read_from_file ( posfile, argv[3], pos_results );
  read_from_file ( negfile, argv[3], neg_results );

  double tp = 0;
  double fp = 0;
  double fn = 0;
  double tn = 0;
  for ( unsigned int i = 0 ; i < pos_results.size(); i++ )
    {
      if ( pos_results[i] == 1 )
        {
          tp++;
        }
      else
        {
          fn++;
        }
    }
  tp /= pos_results.size();
  fn /= pos_results.size();

  for ( unsigned int i = 0 ; i < neg_results.size(); i++ )
    {
      if ( neg_results[i] == 0 )
        {
          tn++;
        }
      else
        {
          fp++;
        }
    }
  tn /= neg_results.size();
  fp /= neg_results.size();

  double accuracy = ( tp + tn ) / ( tp + tn + fp + fn );
  double precision = tp / ( tp + fp );
  cout << "tp: " << tp << endl;
  cout << "tn: " << tn << endl;
  cout << "fn: " << fn << endl;
  cout << "fp: " << fp << endl;
  cout << "accuracy: " << accuracy << endl;
  cout << "precision: " << precision << endl;
  FileStorage fsr ( argv[4], FileStorage::WRITE );
  fsr << "accuracy" << accuracy << "precision" << precision << "tp" << tp << "tn" << tn << "fp" << fp << "fn" << fn;
  fsr.release();
  return 0;
}