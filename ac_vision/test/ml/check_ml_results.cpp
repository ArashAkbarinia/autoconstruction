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

void save_to_file ( string file_name, vector< int > results )
{
  string rect_file = file_name;
  rect_file.append ( "_ml.yaml" );
  FileStorage fsr ( rect_file, FileStorage::WRITE );
  fsr << "new_results" << "[";
  for ( unsigned int i = 0; i < results.size(); i++ )
    {
      fsr << "{:" << "f" << results[i] << "}";
    }
  fsr.release();
}

int main ( int argc, char** argv )
{
  int label = atoi ( argv[1] );
  string filesuffix = "";
  if ( label == 1 )
    {
      filesuffix = "test_pos";
    }
  else
    {
      filesuffix = "test_neg";
    }
  string input = argv[2];
  string output_removed = argv[3];
  string output_equals = argv[4];
  vector< vector< int > > results;
  vector< string > classifier_cleanedup_names;
  vector< string > classifier_removed_names;
  vector< string > classifier_equals_names;
  for ( int i = 5; i < argc; i = i + 2 )
    {
      string tmpfile_name = "cleanedup";
      tmpfile_name.append ( argv[i + 1] );
      tmpfile_name.append ( StringUtils::inttostr ( i ) );
      classifier_cleanedup_names.push_back ( tmpfile_name );
      string tmpfile_name2 = "removed";
      tmpfile_name2.append ( argv[i + 1] );
      tmpfile_name2.append ( StringUtils::inttostr ( i ) );
      classifier_removed_names.push_back ( tmpfile_name2 );
      string tmpfile_name3 = "equals";
      tmpfile_name3.append ( argv[i + 1] );
      tmpfile_name3.append ( StringUtils::inttostr ( i ) );
      classifier_equals_names.push_back ( tmpfile_name3 );

      vector< int > cur_results;
      read_from_file ( argv[i], argv[i + 1], cur_results );
      results.push_back ( cur_results );
    }

  cout << "Number of read files: " << results.size() << endl;
  cout << "Number of images: " << results[0].size() << endl;
  vector< vector< int > > cleanedup_results ( classifier_cleanedup_names.size() );
  vector< vector< int > > removed_results ( classifier_cleanedup_names.size() );
  vector< vector< int > > equals_results ( classifier_cleanedup_names.size() );
  for ( unsigned int i = 0; i < results[0].size(); i++ )
    {
      unsigned int nopposite = 0;
      for ( unsigned int j = 0; j < results.size(); j++ )
        {
          if ( results[j][i] != label )
            {
              nopposite++;
            }
        }
      cout << "[" << i << "]: " << "nopposite " << nopposite << endl;
      if ( nopposite > ( results.size() / 2 ) )
        {
          string file_name = input;
          file_name.append ( filesuffix );
          file_name.append ( StringUtils::inttostr ( i + 1, 5 ) );
          file_name.append ( ".png" );
          string command = "mv ";
          command.append ( file_name );
          command.append ( " " );
          command.append ( output_removed );
          cout << "Moving file: " << file_name << endl;
          system ( command.c_str() );
        }
      else if ( nopposite == ( results.size() / 2 ) )
        {
          string file_name = input;
          file_name.append ( filesuffix );
          file_name.append ( StringUtils::inttostr ( i + 1, 5 ) );
          file_name.append ( ".png" );
          string command = "mv ";
          command.append ( file_name );
          command.append ( " " );
          command.append ( output_equals );
          cout << "Moving file: " << file_name << endl;
          system ( command.c_str() );
        }
      for ( unsigned int j = 0; j < cleanedup_results.size(); j++ )
        {
          if ( nopposite > ( results.size() / 2 ) )
            {
              removed_results[j].push_back ( results[j][i] );
            }
          else if ( nopposite == ( results.size() / 2 ) )
            {
              equals_results[j].push_back ( results[j][i] );
            }
          else
            {
              cleanedup_results[j].push_back ( results[j][i] );
            }
        }
    }

  for ( unsigned int i = 0; i < cleanedup_results.size(); i++ )
    {
      save_to_file ( classifier_cleanedup_names[i], cleanedup_results[i] );
      save_to_file ( classifier_removed_names[i], removed_results[i] );
      save_to_file ( classifier_equals_names[i], equals_results[i] );
    }
  return 0;
}