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

#include "ApplicationSettings.h"

#include "opencv/CvUtils.h"
#include "../ac_vision_constants.h"

#include <ros/package.h>

/// torso
double ApplicationSettings::torso_proportion;
double ApplicationSettings::torso_scaling;

/// safety jackets
double ApplicationSettings::safety_jacket_min_area;
double ApplicationSettings::safety_jacket_max_area;
double ApplicationSettings::safety_jacket_exception_area;
double ApplicationSettings::safety_jacket_intersection_percent1;
double ApplicationSettings::safety_jacket_intersection_percent2;

/// tracking
int ApplicationSettings::filter_type;
int ApplicationSettings::update_type;
double ApplicationSettings::closeness_threshold;
double ApplicationSettings::closeness_area_threshold;
int ApplicationSettings::num_particles;

double ApplicationSettings::tracking_histogram_confidence;
double ApplicationSettings::tracking_safety_jacket_confidence;
double ApplicationSettings::tracking_sift_confidence;

/// misc
int ApplicationSettings::counter;
int ApplicationSettings::waiting_time;

ApplicationSettings::ApplicationSettings ( void )
{

}

void ApplicationSettings::initialise ( string filename )
{
  FileStorage fs;
  string filepath = ros::package::getPath ( AC_VISION_NODE );
  filepath.append ( SETTINGS_FOLDER );
  filepath.append ( filename );
  fs.open ( filepath, FileStorage::READ );

  /// torso
  torso_proportion = ( double ) fs["torso_proportion"];
  torso_scaling = ( double ) fs["torso_scaling"];

  /// safety jacket
  safety_jacket_min_area = ( double ) fs["safety_jacket_min_area"];
  safety_jacket_max_area = ( double ) fs["safety_jacket_max_area"];
  safety_jacket_exception_area = ( double ) fs["safety_jacket_exception_area"];
  safety_jacket_intersection_percent1 = ( double ) fs["safety_jacket_intersection_percent1"];
  safety_jacket_intersection_percent2 = ( double ) fs["safety_jacket_intersection_percent2"];

  /// tracking
  filter_type = ( int ) fs["filter_type"];
  update_type = ( int ) fs["update_type"];
  closeness_threshold = ( double ) fs["closeness_threshold"];
  closeness_area_threshold = ( double ) fs["closeness_area_threshold"];
  num_particles = ( int ) fs["num_particles"];

  tracking_histogram_confidence = ( double ) fs["tracking_histogram_confidence"];
  tracking_safety_jacket_confidence = ( double ) fs["tracking_safety_jacket_confidence"];
  tracking_sift_confidence = ( double ) fs["tracking_sift_confidence"];

  /// misc
  counter = ( int ) fs["counter"];
  waiting_time = ( int ) fs["waiting_time"];

  fs.release();
}
