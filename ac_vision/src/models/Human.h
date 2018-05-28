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

#ifndef HUMAN_H
#define HUMAN_H

#include "../utils/opencv/CvUtils.h"

class Human
{

public:

  Human ( void );
  Human ( const Rect current_boundry );

  Rect get_current_boundry ( void ) const;
  Point get_current_position ( void );
  vector< Rect > get_history_boundries ( void );

  void update_current_boundry ( const Rect current_boundry );
  void set_current_boundry ( const Rect current_boundry );
  void set_current_position ( Point current_position );

  void add_safety_jacket_detection ( const bool is_detected );
  void add_sift_detection ( const bool is_detected );
  void set_safety_jacket_detection ( const bool is_detected );

  bool does_safety_jacket_agree ( void );
  bool does_sift_agree ( void );

  vector< Mat > histograms;

private:

  void add_to_history ( void );
  Rect current_boundry;
  Point current_position;
  // FIXME: only keep the history for 100 frames
  vector< Rect > history_boundries;
  vector< vector< bool > > detections;

  double safety_jacket_counter;
  double sift_counter;

};

#endif // HUMAN_H