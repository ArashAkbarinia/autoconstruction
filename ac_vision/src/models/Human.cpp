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

#include "Human.h"

#include "../utils/ApplicationSettings.h"
#include "../ac_vision_constants.h"

Human::Human ( void )
{
  Human ( Rect ( 0, 0, 0, 0 ) );
}

Human::Human ( const Rect current_boundry )
{
  update_current_boundry ( current_boundry );

  safety_jacket_counter = 0;
  sift_counter = 0;

  // [0] for safety jackets
  detections.push_back ( vector< bool >() );
  // [1] for sift
  detections.push_back ( vector< bool >() );
}

Rect Human::get_current_boundry ( void  ) const
{
  return current_boundry;
}

Point Human::get_current_position ( void )
{
  return current_position;
}

vector< Rect > Human::get_history_boundries ( void )
{
  return history_boundries;
}

void Human::update_current_boundry ( const Rect current_boundry )
{
  this->current_boundry = current_boundry;
}

void Human::set_current_boundry ( const Rect current_boundry )
{
  add_to_history ( );
  this->current_boundry = current_boundry;
}

void Human::add_safety_jacket_detection ( const bool is_detected )
{
  if ( detections[0].size() >= 100 )
    {
      if ( detections[0][0] )
        {
          safety_jacket_counter--;
        }
      detections[0].erase ( detections[0].begin() );
    }
  detections[0].push_back ( is_detected );
  if ( is_detected )
    {
      safety_jacket_counter++;
    }
}

void Human::add_sift_detection ( const bool is_detected )
{
  if ( detections[1].size() >= 100 )
    {
      if ( detections[1][0] )
        {
          sift_counter--;
        }
      detections[1].erase ( detections[1].begin() );
    }
  detections[1].push_back ( is_detected );
  if ( is_detected )
    {
      sift_counter++;
    }
}

void Human::set_safety_jacket_detection ( const bool is_detected )
{
  detections[0].back() = is_detected;
  if ( is_detected )
    {
      safety_jacket_counter++;
    }
}

bool Human::does_safety_jacket_agree ( void )
{
  if ( detections[0].size() >= 10 )
    {
      double percent = safety_jacket_counter / ( double ) detections[0].size();
      if ( percent <= ApplicationSettings::tracking_safety_jacket_confidence )
        {
          LOG_DEBUG ( "Human::does_safety_jacket_agree: " << percent );
          return false;
        }
    }
  return true;
}

bool Human::does_sift_agree ( void )
{
  if ( detections[1].size() >= 10 )
    {
      double percent = sift_counter / ( double ) detections[1].size();
      if ( percent <= ApplicationSettings::tracking_sift_confidence )
        {
          LOG_DEBUG ( "Human::does_sift_agree: " << percent );
          return false;
        }
    }
  return true;
}

void Human::add_to_history ( void )
{
  if ( history_boundries.size() >= 100 )
    {
      history_boundries.erase ( history_boundries.begin() );
    }
  history_boundries.push_back ( current_boundry );
}

void Human::set_current_position ( Point current_position )
{
  this->current_position = current_position;
}