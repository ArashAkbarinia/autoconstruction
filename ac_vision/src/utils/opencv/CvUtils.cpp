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

#include "CvUtils.h"
#include "../ApplicationSettings.h"

CvUtils::CvUtils ( void )
{

}

bool CvUtils::are_rects_subsets ( Rect a, Rect b ) const
{
  Rect union_rect = a & b;
  return union_rect == a || union_rect == b;
}

void CvUtils::draw_detected_humans ( Mat& image, const vector< Rect > detected_humans, const Scalar colour ) const
{
  for ( unsigned int i = 0; i < detected_humans.size(); i++ )
    {
      rectangle ( image, detected_humans[i], colour );
    }
}

void CvUtils::draw_detected_humans ( Mat& image, const vector< SafetyJacket > detected_humans, const Scalar colour ) const
{
  for ( unsigned int i = 0; i < detected_humans.size(); i++ )
    {
      rectangle ( image, detected_humans[i].human, colour );
    }
}

Rect CvUtils::enlarge_window ( const Rect orgrect, const Mat& image, const double scale ) const
{
  Rect window;
  Point center = get_rect_centre ( orgrect );

  window = Rect ( center.x - scale * orgrect.width / 2, center.y - scale * orgrect.height / 2, scale * orgrect.width, scale * orgrect.height );

  Rect bounds ( 0, 0, image.cols, image.rows );
  window = window & bounds;

  return window;
}

Rect CvUtils::enlarge_window_height ( Rect orgrect, Mat image, double scale )
{
  Rect window;
  Point center = get_rect_centre ( orgrect );

  window = Rect ( orgrect.x, center.y - scale * orgrect.height / 2, orgrect.width, scale * orgrect.height );

  Rect bounds ( 0, 0, image.cols, image.rows );
  window = window & bounds;

  return window;
}

Rect CvUtils::enlarge_window_width ( Rect orgrect, Mat image, double scale )
{
  Rect window;
  Point center = get_rect_centre ( orgrect );

  window = Rect ( center.x - scale * orgrect.width / 2, orgrect.y, scale * orgrect.width, orgrect.height );

  Rect bounds ( 0, 0, image.cols, image.rows );
  window = window & bounds;

  return window;
}

Rect CvUtils::resize_window ( const Rect orgsquare, const Mat& image, const Size size ) const
{
  Rect window;
  Rect bounds ( 0, 0, image.cols, image.rows );

  window = bounds & orgsquare;
  window.width = size.width;
  window.height = size.height;

  // x
  if ( window.x - ( size.width / 2 ) >= 0 )
    {
      window.x -= size.width / 2;
    }
  else
    {
      window.x = 0;
    }
  if ( window.br().x >= image.cols )
    {
      window.x -= ( window.br().x - image.cols - 1 );
    }

  // y
  if ( window.y - ( size.height / 2 ) >= 0 )
    {
      window.y -= size.height / 2;
    }
  else
    {
      window.y = 0;
    }
  if ( window.br().y >= image.rows )
    {
      window.y -= ( window.br().y - image.rows - 1 );
    }

  return window;
}

Rect CvUtils::squareise_window ( const Rect orgrect, const Mat& image ) const
{
  Rect window;
  Rect bounds ( 0, 0, image.cols, image.rows );

  window = bounds & orgrect;

  if ( window.width == window.height )
    {
      return window;
    }

  int side = MAX ( window.width, window.height );
  // TODO: it's nices to centeralise the square
  if ( side >= image.rows || side >= image.cols )
    {
      side = MIN ( image.rows, image.cols ) - 1;
      if ( image.rows < image.cols )
        {
          window.y = 0;
        }
      else
        {
          window.x = 0;
        }
    }
  else if ( window.width < side )
    {
      int diff = side - window.width;
      if ( diff % 2 != 0 )
        {
          diff++;
        }
      if ( window.x - ( diff / 2 ) >= 0 && window.br().x + ( diff / 2 ) < image.cols )
        {
          window.x -= ( diff / 2 );
        }
      else if ( window.x - ( diff / 2 ) < 0 )
        {
          window.x = 0;
        }
      else
        {
          window.x = ( window.br().x - side );
        }
    }
  else
    {
      int diff = side - window.height;
      if ( diff % 2 != 0 )
        {
          diff++;
        }
      if ( window.y - ( diff / 2 ) >= 0 && window.br().y + ( diff / 2 ) < image.rows )
        {
          window.y -= ( diff / 2 );
        }
      else if ( window.y - ( diff / 2 ) < 0 )
        {
          window.y = 0;
        }
      else
        {
          window.y = ( window.br().y - side );
        }
    }

  window.width = side;
  window.height = side;
  return window;
}

void CvUtils::squareise_windows ( vector< Rect >& rects, const Mat& image ) const
{
  for ( unsigned int i = 0; i < rects.size(); i++ )
    {
      rects[i] = squareise_window ( rects[i], image );
    }
}

void CvUtils::squareise_windows ( vector< SafetyJacket >& jackets, const Mat& image ) const
{
  for ( unsigned int i = 0; i < jackets.size(); i++ )
    {
      jackets[i].human = squareise_window ( jackets[i].human, image );
    }
}

Mat CvUtils::get_upperbody ( const Rect torso, const Mat& image, Rect& upperbody_roi ) const
{
  upperbody_roi = get_upperbody_rect ( torso, image );

  Mat upperbody ( image, upperbody_roi );
  resize ( upperbody, upperbody, Size ( 64, 64 ), 0, 0, INTER_CUBIC );
  return upperbody;
}

Rect CvUtils::get_upperbody_rect ( const Rect torso, const Mat& image ) const
{
  Rect torsoaquare = squareise_window ( torso, image );

  Rect upperbody_roi;
  // in an upperbody: 1/3 is head and 2/3 is torso
  upperbody_roi.x = torsoaquare.x - ( torsoaquare.width / ( ApplicationSettings::torso_proportion * 2 ) );
  upperbody_roi.y = torsoaquare.y - ( torsoaquare.height / ApplicationSettings::torso_proportion );
  upperbody_roi.width = torsoaquare.width + ( torsoaquare.height / ApplicationSettings::torso_proportion );
  upperbody_roi.height = torsoaquare.height + ( torsoaquare.height / ApplicationSettings::torso_proportion );

  upperbody_roi = squareise_window ( upperbody_roi, image );
  upperbody_roi = enlarge_window ( upperbody_roi, image, ApplicationSettings::torso_scaling );
  return upperbody_roi;
}

void CvUtils::get_upperbody_rects ( vector< Rect >& rects, const Mat& image ) const
{
  for ( unsigned int i = 0; i < rects.size(); i++ )
    {
      rects[i] = get_upperbody_rect ( rects[i], image );
    }
}

void CvUtils::get_upperbody_rects ( vector< SafetyJacket >& jackets, const Mat& image ) const
{
  for ( unsigned int i = 0; i < jackets.size(); i++ )
    {
      jackets[i].human = get_upperbody_rect ( jackets[i].human, image );
    }
}

double CvUtils::euclidean_distance ( const Rect a, const Rect b ) const
{
  return euclidean_distance ( get_rect_centre ( a ), get_rect_centre ( b ) );
}

double CvUtils::euclidean_distance ( const Point a, const Point b ) const
{
  return sqrt ( pow ( a.x - b.x, 2 ) + pow ( a.y - b.y, 2 ) );
}

double CvUtils::euclidean_distance ( Point3f a, Point3f b )
{
  return sqrt ( pow ( a.x - b.x, 2 ) + pow ( a.y - b.y, 2 ) + pow ( a.z - b.z, 2 ) );
}

Point CvUtils::get_rect_centre ( const Rect rect ) const
{
  Point centre;
  centre.x = rect.x + ( rect.width * 0.5 );
  centre.y = rect.y + ( rect.height * 0.5 );
  return centre;
}

Mat CvUtils::img2bw ( Mat image )
{
  if ( image.channels() == 3 )
    {
      cvtColor ( image, image, CV_RGB2GRAY );
    }
  Mat im_bw;
  threshold ( image, im_bw, 128, 255, CV_THRESH_BINARY );
  return im_bw;
}

Circle CvUtils::merge_circles ( Circle a, Circle b  ) const
{
  vector< Point2f > points ( 8 );
  // circle a
  points[0] = Point2f ( a.location.x, a.location.y + a.radius );
  points[1] = Point2f ( a.location.x, a.location.y - a.radius );
  points[2] = Point2f ( a.location.x + a.radius, a.location.y );
  points[3] = Point2f ( a.location.x - a.radius, a.location.y );
  // circle b
  points[4] = Point2f ( b.location.x, b.location.y + b.radius );
  points[5] = Point2f ( b.location.x, b.location.y - b.radius );
  points[6] = Point2f ( b.location.x + b.radius, b.location.y );
  points[7] = Point2f ( b.location.x - b.radius, b.location.y );

  Point2f centre;
  float radius;
  minEnclosingCircle ( points, centre, radius );
  Circle result ( centre, radius );
  return result;
}

Rect CvUtils::square_of_circle ( Point2f centre, float radius ) const
{
  float side = radius * 2;
  Rect square ( centre.x - radius, centre.y - radius, side, side );
  return square;
}

Rect CvUtils::square_of_circle ( Point2f centre, float radius, const Mat image ) const
{
  Rect image_bound ( 0, 0, image.cols, image.rows );
  return square_of_circle ( centre, radius, image_bound );
}

Rect CvUtils::square_of_circle ( Point2f centre, float radius, const Rect bound ) const
{
  Rect square = square_of_circle ( centre, radius );
  square &= bound;
  return square;
}

Mat CvUtils::correct_progo_image ( const Mat image, const float rescale ) const
{
  static float camera_matrix_vector[3][3] = { {469.967, 0, 640}, {0, 467.682, 360}, {0, 0, 1} };
  static Mat camera_matrix ( 3, 3, CV_32FC1, camera_matrix_vector );
  static float dist_coeffs_vector[5] = { -0.18957, 0.03732, 0, 0, -0.003367 };
  static Mat dist_coeffs ( 1, 5, CV_32FC1, dist_coeffs_vector );

  Mat frame;
  undistort ( image, frame, camera_matrix, dist_coeffs );
  flip ( frame, frame, -1 );
  resize ( frame, frame, Size(), rescale, rescale, INTER_CUBIC );

  return frame;
}
