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

#ifndef CVUTILS_H
#define CVUTILS_H

#include "colours.h"
#include "../../models/models.h"

#include <iostream>

#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <vector>

/**
* @class CvUtils
*
* @brief This class contains useful functions for the image processing part.
*
* @author Arash Akbarinia
*
*/

using namespace cv;
using namespace std;

class CvUtils
{

public:
  CvUtils ( void );

  bool are_rects_subsets ( Rect a, Rect b ) const;

  void draw_detected_humans ( Mat& image, const vector< Rect > detected_humans, const Scalar colour = BLACK ) const;
  void draw_detected_humans ( Mat& image, const vector< SafetyJacket > detected_humans, const Scalar colour = BLACK ) const;

  Rect enlarge_window ( const Rect orgrect, const Mat& image, const double scale = 2.0 ) const;
  Rect enlarge_window_height ( Rect orgrect, Mat image, double scale = 2.0 );
  Rect enlarge_window_width ( Rect orgrect, Mat image, double scale = 2.0 );
  Rect resize_window ( const Rect orgsquare, const Mat& image, const Size size ) const;
  Rect squareise_window ( const Rect orgrect, const Mat& image ) const;
  void squareise_windows ( vector< Rect >& rects, const Mat& image ) const;
  void squareise_windows ( vector< SafetyJacket >& jackets, const Mat& image ) const;
  Mat get_upperbody ( const Rect torso, const Mat& image, Rect& upperbody_roi ) const;
  Rect get_upperbody_rect ( const Rect torso, const Mat& image ) const;
  void get_upperbody_rects ( vector< Rect >& rects, const Mat& image ) const;
  void get_upperbody_rects ( vector< SafetyJacket >& jackets, const Mat& image ) const;

  double euclidean_distance ( const Rect a, const Rect b ) const;
  double euclidean_distance ( const Point a, const Point b ) const;
  double euclidean_distance ( Point3f a, Point3f b );

  Point get_rect_centre ( const Rect rect ) const;

  Mat img2bw ( Mat image );

  Circle merge_circles ( Circle a, Circle b ) const;

  Rect square_of_circle ( Point2f centre, float radius ) const;
  Rect square_of_circle ( Point2f centre, float radius, const Mat image ) const;
  Rect square_of_circle ( Point2f centre, float radius, const Rect bound ) const;

  Mat correct_progo_image ( const Mat image, const float rescale = 0.5 ) const;

};

#endif // CVUTILS_H
