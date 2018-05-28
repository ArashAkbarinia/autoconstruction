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


#include "ApplicationSession.h"
#include "ApplicationSettings.h"
#include "StringUtils.h"

#include <ros/ros.h>

ApplicationSession::ApplicationSession ( int argc, char** argv )
{
  is_video_mode = true;
  is_video_writer_initialised = false;
  num_frames = 0;
  end_second = 0;
  starting_second = 0;
  settings_file =  "application_settings.yaml";
  parse_command_line ( argc, argv );
  ApplicationSettings::initialise ( settings_file );
  process_input_file ( );
}

void ApplicationSession::add_frame_to_video_writer ( Mat frame )
{
  if ( !is_video_writer_initialised )
    {
      initialise_output_file ( Size ( frame.cols, frame.rows ) );
    }
  video_writer << frame;
}

unsigned int ApplicationSession::get_num_frames ( void )
{
  return num_frames;
}

string ApplicationSession::get_output_file ( void )
{
  return output_file;
}

void ApplicationSession::initialise_output_file ( Size size )
{
  is_video_writer_initialised = true;
  video_writer.open ( output_file, CV_FOURCC ( 'D', 'I', 'V', 'X' ), 60, size );
  if ( !video_writer.isOpened ( ) )
    {
      cerr << "Can't open file [" << output_file << "]." << endl;
    }
}

void ApplicationSession::parse_command_line ( int argc, char** argv )
{
  int c = -1;
  while ( ( c = getopt ( argc, argv, "c:e:fi:o:s:" ) ) != -1 )
    {
      switch ( c )
        {
        case 'c':
          settings_file = optarg;
          break;
        case 'e':
          end_second = atoi ( optarg );
          break;
        case 'f':
          is_video_mode = false;
          break;
        case 'i':
          input_file = optarg;
          break;
        case 'o':
          output_file = optarg;
          break;
        case 's':
          starting_second = atoi ( optarg );
          break;
        default:
          cerr << "Usage: " << argv[0] << " [-c configurations_file] [-e end_second] [-f] [-i input_file] [-o output_file] [-s starting_second] " << endl << endl;
          cerr << "\t[-c configurations_file]: configurations file, if not specified uses the default one." << endl;
          cerr << "\t[-e end_second]: finishing the process at this second." << endl;
          cerr << "\t[-f]: if the input_file is txt." << endl;
          cerr << "\t[-i input_file]: input file, can be either video format or txt." << endl;
          cerr << "\t[-o output file]: desired name of output file." << endl;
          cerr << "\t[-s starting_second]: starting the process from this second." << endl;
          exit ( 1 );
        }
    }
}

void ApplicationSession::process_input_file ( void )
{
  if ( is_video_mode )
    {
      if ( !video_capture.open ( input_file ) )
        {
          cerr << "Can't open file [" << input_file << "]." << endl;
          exit ( 1 );
        }
      // TODO: check if the startig second is not out of bound and do propper logging
      video_capture.set ( CV_CAP_PROP_POS_MSEC, starting_second * 1000 );
    }
  else
    {
      file = fopen ( input_file.c_str ( ), "r" );
      if ( file == NULL )
        {
          cerr << "Can't open file [" << input_file << "]." << endl;
          exit ( 1 );
        }
    }
  cout << "Processing file [" << input_file << "]." << endl;
}

Mat ApplicationSession::read_one_frame ( void )
{
  Mat frame;
  num_frames++;
  // extracting frames from video.
  if ( is_video_mode )
    {
      double cursecond = video_capture.get ( CV_CAP_PROP_POS_MSEC ) / 1000;
      if ( end_second == 0 || cursecond < end_second )
        {
          video_capture >> frame;
        }
      return frame;
    }
  else
    {
      // extracting frames based on .txt file.
      char filename[FILE_LENGTH];
      while ( file )
        {
          if ( !fgets ( filename, FILE_LENGTH - 2, file ) )
            {
              return frame;
            }
          if ( filename[0] == '#' )
            {
              continue;
            }
          int l = strlen ( filename );
          while ( l > 0 && isspace ( filename[l - 1] ) )
            {
              --l;
            }
          filename[l] = '\0';
          frame = imread ( filename );
          return frame;
        }
    }
  return frame;
}
