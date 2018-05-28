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

#ifndef GTUTILS_H
#define GTUTILS_H

#include "../../../src/utils/opencv/CvUtils.h"

class GtUtils
{

public:
  static void compare_gts_results ( const vector< vector< Point > > gts, const vector< vector< Point > > results, const string output_file );
  static void create_combine_gts_vector ( const vector< string > filenames, vector< vector< Point> >& gts );
  static void create_combine_gts_vector ( const vector< string > filenames, vector< vector< Rect> >& gts );

private:
  static ConfusionMatrix data_association ( const vector< Point > gts, const vector< Point > results, vector< Point >& matching, vector< Point >& outliers );
  static void compare_gts_results ( const vector< Point > gts, const vector< Point > results, const string filename );

};

#endif // GTUTILS_H