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

#include "ForegroundExtractor.h"

ForegroundExtractor::ForegroundExtractor ( void )
{
  bs_gmg.decisionThreshold = 0.7;
  bs_gmg.numInitializationFrames = 5;
}

Mat ForegroundExtractor::get_foreground ( Mat image, BackgroundSubtractor& background_subtractor )
{
  Mat foreground_bw;
  background_subtractor ( image, foreground_bw, -1.0 );
  Mat foreground = Mat::zeros ( image.rows, image.cols, image.type() );
  image.copyTo ( foreground, foreground_bw );
  return foreground;
}

Mat ForegroundExtractor::get_foreground_gmg ( Mat image )
{
  return get_foreground ( image, bs_gmg );
}

Mat ForegroundExtractor::get_foreground_mog ( Mat image )
{
  return get_foreground ( image, bs_mog );
}

Mat ForegroundExtractor::get_foreground_mog2 ( Mat image )
{
  return get_foreground ( image, bs_mog2 );
}
