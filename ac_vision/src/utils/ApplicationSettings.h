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

#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <string>

using namespace std;

class ApplicationSettings
{

public:

  ApplicationSettings ( void );
  static void initialise ( string filename );

  /// torso
  static double torso_proportion;
  static double torso_scaling;

  /// safety jackets
  static double safety_jacket_max_area;
  static double safety_jacket_min_area;
  static double safety_jacket_exception_area;
  static double safety_jacket_intersection_percent1;
  static double safety_jacket_intersection_percent2;

  /// tracking
  static int filter_type;
  static int update_type;
  static double closeness_threshold;
  static double closeness_area_threshold;
  static int num_particles;

  static double tracking_histogram_confidence;
  static double tracking_safety_jacket_confidence;
  static double tracking_sift_confidence;

  /// misc
  static int counter;
  static int waiting_time;

};

#endif // APPLICATIONSETTINGS_H
