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

#include "FileUtils.h"

#include <algorithm>
#include <dirent.h>

// TODO: add option for the types of file to be read, e.g. .jpg, .png
// TODO: add the return value for number of read files in the directory
void FileUtils::read_directory ( const string& directory_name, vector< string >& filenames, bool add_directory_name )
{
  filenames.clear ( );

  DIR* dir = opendir ( directory_name.c_str ( ) );
  if ( dir != NULL )
    {
      struct dirent* dent;
      while ( ( dent = readdir ( dir ) ) != NULL )
        {
          string curfilename = string ( dent->d_name );
          if ( string::npos == curfilename.find ( ".jpg" ) && string::npos == curfilename.find ( ".png" ) &&
               string::npos == curfilename.find ( ".JPG" ) && string::npos == curfilename.find ( ".PNG" ) )
            {
              continue;
            }
          if ( add_directory_name )
            {
              filenames.push_back ( directory_name + "/" + curfilename );
            }
          else
            {
              filenames.push_back ( curfilename );
            }
        }
    }

  sort ( filenames.begin ( ), filenames.end ( ) );
}

void FileUtils::read_rects_from_file ( const string file_name, vector< Rect >& rois )
{
  FileStorage fs ( file_name, FileStorage::READ );
  FileNode features = fs["rois"];
  FileNodeIterator itb = features.begin();
  FileNodeIterator ite = features.end();

  for ( ; itb != ite; ++itb )
    {
      int x = ( int ) ( *itb ) ["x"];
      int y = ( int ) ( *itb ) ["y"];
      int w = ( int ) ( *itb ) ["w"];
      int h = ( int ) ( *itb ) ["h"];
      rois.push_back ( Rect ( x, y, w, h ) );
    }
  fs.release();
}

void FileUtils::read_results_from_file ( const string file_name, vector< vector< Rect > >& rois )
{
  FileStorage fs ( file_name, FileStorage::READ );
  FileNode centers = fs["rois"];
  FileNodeIterator it = centers.begin(), it_end = centers.end();

  for ( ; it != it_end; ++it )
    {
      int length = ( int ) ( *it ) ["length"];
      FileNode points_fn = ( *it ) ["rects"];
      FileNodeIterator itb = points_fn.begin();
      FileNodeIterator ite = points_fn.end();
      vector< Rect > rects ( length );
      for ( unsigned i = 0 ; itb != ite; ++itb, i++ )
        {
          int x = ( int ) ( *itb ) ["x"];
          int y = ( int ) ( *itb ) ["y"];
          int w = ( int ) ( *itb ) ["w"];
          int h = ( int ) ( *itb ) ["h"];
          rects[i] = Rect ( x, y, w, h );
        }
      rois.push_back ( rects );
    }
  fs.release();
}

void FileUtils::read_results_from_file ( const string file_name, vector< vector< Point > >& points )
{
  FileStorage fs ( file_name, FileStorage::READ );
  FileNode centers = fs["rois"];
  FileNodeIterator it = centers.begin(), it_end = centers.end();

  CvUtils cv_utils;
  for ( ; it != it_end; ++it )
    {
      int length = ( int ) ( *it ) ["length"];
      FileNode points_fn = ( *it ) ["rects"];
      FileNodeIterator itb = points_fn.begin();
      FileNodeIterator ite = points_fn.end();
      vector< Point > curr_points ( length );
      for ( unsigned i = 0 ; itb != ite; ++itb, i++ )
        {
          int x = ( int ) ( *itb ) ["x"];
          int y = ( int ) ( *itb ) ["y"];
          int w = ( int ) ( *itb ) ["w"];
          int h = ( int ) ( *itb ) ["h"];
          curr_points[i] = cv_utils.get_rect_centre ( Rect ( x, y, w, h ) );
        }
      points.push_back ( curr_points );
    }
  fs.release();
}

void FileUtils::write_distances_to_file ( const string file_name, const vector< double > distances, const double mse, const double mean )
{
  string distances_file = file_name;
  distances_file.append ( "-distances.yaml" );
  FileStorage fsr ( distances_file, FileStorage::WRITE );
  fsr << "stdv " << sqrt ( mse );
  fsr << "mean " << mean;
  fsr << "distance" << "[";
  for ( unsigned int i = 0; i < distances.size(); i++ )
    {
      fsr << "{:" << "d" << distances[i] << "}";
    }
  fsr.release();
}

void FileUtils::write_rects_to_file ( const string file_name, const vector< Rect > rois )
{
  string rect_file = file_name;
  rect_file.append ( "_rois.yaml" );
  FileStorage fsr ( rect_file, FileStorage::WRITE );
  fsr << "rois" << "[";
  // TODO: write the rectangle itself instead of one by one
  for ( unsigned int i = 0; i < rois.size(); i++ )
    {
      fsr << "{:" << "x" << rois[i].x << "y" << rois[i].y << "w" << rois[i].width << "h" << rois[i].height << "}";
    }
  fsr.release();
}

void FileUtils::write_results_to_file ( const string file_name, const vector< vector< Rect > > rois )
{
  string rect_file = file_name;
  rect_file.append ( "_results.yaml" );
  FileStorage fsr ( rect_file, FileStorage::WRITE );
  fsr << "rois" << "[";
  for ( unsigned int i = 0; i < rois.size(); i++ )
    {
      fsr << "{:" << "length" << ( int ) rois[i].size() << "rects" << "[:";
      for ( unsigned int j = 0; j < rois[i].size(); j++ )
        {
          fsr << "{:" << "x" << rois[i].at ( j ).x << "y" << rois[i].at ( j ).y << "w" << rois[i].at ( j ).width << "h" << rois[i].at ( j ).height << "}";
        }
      fsr << "]" << "}";
    }
  fsr.release();
}

void FileUtils::write_result_report_to_file ( const string file_name, const vector< vector< Point > > outliers, const double outliers_ratio, const ConfusionMatrix confusion_matrix )
{
  string centre_file = file_name;
  centre_file.append ( "-evaluation.yaml" );
  FileStorage fsc ( centre_file, FileStorage::WRITE );
  double accuracy = ( double ) ( confusion_matrix.tp + confusion_matrix.tn ) /
                    ( double ) ( confusion_matrix.tp + confusion_matrix.tn + confusion_matrix.fp + confusion_matrix.fn );
  double error_rate = ( double ) ( confusion_matrix.fp + confusion_matrix.fn ) /
                      ( double ) ( confusion_matrix.tp + confusion_matrix.tn + confusion_matrix.fp + confusion_matrix.fn );
  double precision = ( double ) ( confusion_matrix.tp ) / ( double ) ( confusion_matrix.tp + confusion_matrix.fp );
  double tpr = ( double ) confusion_matrix.tp / ( double ) ( confusion_matrix.tp + confusion_matrix.fn );
  double fpr = ( double ) confusion_matrix.fp / ( double ) ( confusion_matrix.tn + confusion_matrix.fp );
  double fnr = ( double ) confusion_matrix.fn / ( double ) ( confusion_matrix.tp + confusion_matrix.fn );
  double tnr = ( double ) confusion_matrix.tn / ( double ) ( confusion_matrix.tn + confusion_matrix.fp );
  fsc << "True positives" << confusion_matrix.tp << "True negatives" << confusion_matrix.tn
      << "False positives" << confusion_matrix.fp << "False negatives" << confusion_matrix.fn;
  fsc << "TPR" << tpr << "TNR" << tnr
      << "FPR" << fpr << "FNR" << fnr;
  fsc << "Accuracy" << accuracy << "Error rate" << error_rate << "Precision" << precision;
  fsc << "Outliers" << outliers_ratio << "center" << "[";
  for ( unsigned int i = 0; i < outliers.size(); i++ )
    {
      fsc << "{:" << "length" << ( int ) outliers[i].size() << "points" << "[:";
      for ( unsigned int j = 0; j < outliers[i].size(); j++ )
        {
          Point center = outliers[i].at ( j );
          fsc << "{:" << "x" << center.x << "y" << center.y << "}";
        }
      fsc << "]" << "}";
    }
  fsc.release();
}
