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

#ifndef SAFETYJACKETDETECTOR_H
#define SAFETYJACKETDETECTOR_H

#include "CvUtils.h"

class SafetyJacketDetector
{

public:
  SafetyJacketDetector ( void );
  void detect ( const Mat& image, vector< SafetyJacket >& jackets ) const;

protected:
  void find_jackets ( const Mat& binary_image, vector< SafetyJacket >& jackets ) const;

private:
  CvUtils cv_utils;

  void merge_subblobs ( const vector < SafetyJacket >& input, vector < SafetyJacket >& output, const double eps ) const;
  void remove_smallblobs ( const vector< SafetyJacket >& input, vector< SafetyJacket >& output, const float numrows ) const;
  Mat binarise_image ( const Mat& image ) const;

};

#endif // SAFETYJACKETDETECTOR_H