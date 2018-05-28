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

#ifndef APPLICATIONSESSION_H
#define APPLICATIONSESSION_H

#include "../ac_vision_constants.h"
#include "opencv/CvUtils.h"

#define FILE_LENGTH 1024

class ApplicationSession
{

public:

  ApplicationSession ( int argc, char** argv );

  void add_frame_to_video_writer ( Mat frame );
  unsigned int get_num_frames ( void );
  string get_output_file ( void );
  Mat read_one_frame ( void );

private:

  void initialise_output_file ( Size size );
  void parse_command_line ( int argc, char** argv );
  void process_input_file ( void );

  bool is_video_mode;
  bool is_video_writer_initialised;
  unsigned int num_frames;
  unsigned int end_second;
  unsigned int starting_second;
  string input_file;
  string output_file;
  string settings_file;
  VideoCapture video_capture;
  VideoWriter video_writer;
  FILE* file;

};

#endif // APPLICATIONSESSION_H
