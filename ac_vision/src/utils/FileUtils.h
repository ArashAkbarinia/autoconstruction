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


#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "opencv/CvUtils.h"

#include <string>
#include <vector>

class FileUtils
{

public:
  static void read_directory ( const string& directory_name, vector< string >& filenames, bool add_directory_name = true );
  static void read_rects_from_file ( const string file_name, vector< Rect >& rois );
  static void read_results_from_file ( const string file_name, vector< vector< Rect > >& rois );
  static void read_results_from_file ( const string file_name, vector< vector< Point > >& points );
  static void write_distances_to_file ( const string file_name, const vector< double > distances, const double mse, const double mean );
  static void write_rects_to_file ( const string file_name, const vector< Rect > rois );
  static void write_results_to_file ( const string file_name, const vector< vector< Rect > > rois );
  static void write_result_report_to_file ( const string file_name, const vector< vector< Point > > outliers, const double outliers_ratio, const ConfusionMatrix confusion_matrix );

};

#endif // FILEUTILS_H
