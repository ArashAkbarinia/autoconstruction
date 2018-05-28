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


#ifndef COLOURS_H
#define COLOURS_H

#include <opencv2/core/core.hpp>

using namespace cv;

const Scalar BLACK = Scalar ( 0, 0, 0 );
const Scalar BLUE = Scalar ( 255, 0, 0 );
const Scalar CYAN = Scalar ( 255, 255, 0 );
const Scalar GREEN = Scalar ( 0, 255, 0 );
const Scalar MAGENTA = Scalar ( 255, 0, 255 );
const Scalar RED = Scalar ( 0, 0, 255 );
const Scalar WHITE = Scalar ( 255, 255, 255 );
const Scalar YELLOW = Scalar ( 0, 255, 255 );

#endif // COLOURS_H
