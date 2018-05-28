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

#ifndef HUMANTRACKER_H
#define HUMANTRACKER_H

#include "ParticleFilter/StateData.h"
#include "../models/models.h"
#include "../utils/opencv/CvUtils.h"

#include <opencv2/legacy/legacy.hpp>

class HumanTracker
{

public:

  HumanTracker ( void );
  void initialise ( void );
  vector< Rect > track ( const Mat& image, const vector< SafetyJacket > jackets );
  vector< Rect> get_humanrects ( void ) const;

private:

  void track_particle ( const Mat& image, const vector< SafetyJacket > jackets );
  void track_condens ( const Mat& image, const vector< SafetyJacket > jackets );
  bool are_rects_close ( const Rect jacket, const Rect human ) const;

  void initialise_condens ( const Mat& image, const Rect jacket, CvConDensation& condens ) const;
  void update_condenses ( const Mat& image, CvConDensation& condens, const Human human ) const;

  CvUtils cv_utils;

  int filter_type;
  int update_type;
  double closeness_threshold;
  vector< Human > humans;

  /// particle filter
  vector< StateData > state_datas;
  unsigned int num_particles;

  /// condens
  vector< CvConDensation > condenses;

};

#endif // HUMANTRACKER_H
