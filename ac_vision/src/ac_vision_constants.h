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

#ifndef AC_VISION_CONSTANTS_H
#define AC_VISION_CONSTANTS_H

#define AC_VISION_NODE                "ac_vision"

/// folders

// settings
#define SETTINGS_FOLDER               "/resources/settings/"

// trained classifiers
#define CLASSIFIERS_FOLDER            "/resources/classifiers/"

// cascade classifiers
#define HAAR_CASCADE_FOLDER           "/resources/cascades/haarcascades/"
#define HOG_CASCADE_FOLDER            "/resources/cascades/hogcascades/"
#define BGS_CONFIG                    "/resources/bgslibrary/config/"

// hog descriptors
#define HOG_DESCRIPTOR_FOLDER         "/resources/hogdescriptors/"

// latent svm
#define LATENT_SVM_FOLDER             "/resources/latentsvmdetector/"

/// segmentation
#define SAFETY_JACKET_COLOUR_LOW1     Scalar ( 25,  50, 205 )
#define SAFETY_JACKET_COLOUR_HIGH1    Scalar ( 60, 255, 255 )
#define SAFETY_JACKET_COLOUR_LOW2     Scalar ( 25, 150,  90 )
#define SAFETY_JACKET_COLOUR_HIGH2    Scalar ( 60, 255, 255 )

#define SAFETY_JACKET_MORPH_CLOSE     Size ( 7, 7 )
#define SAFETY_JACKET_MORPH_OPEN      Size ( 3, 3 )

#define SAFETY_JACKET_CLOSE_ELEMENT   getStructuringElement ( MORPH_RECT, SAFETY_JACKET_MORPH_CLOSE )
#define SAFETY_JACKET_OPEN_ELEMENT    getStructuringElement ( MORPH_RECT, SAFETY_JACKET_MORPH_OPEN )

#define SAFETY_JACKET_GAUSSIAN_SIZE   Size ( 7, 7 )

/// human detector
#define ML_BRIEF_SVM                  0
#define ML_BRISK_SVM                  1
#define ML_FREAK_SVM                  2
#define ML_HOG_SVM                    3
#define ML_LATENT_SVM                 4
#define ML_SIFT_SVM                   5
#define ML_SURF_SVM                   6

/// logging
#define LOG_ERROR(str)                do { std::cerr << str << std::endl; } while( false )

#ifdef DEBUG
#define LOG_DEBUG(str)                do { std::cout << str << std::endl; } while( false )
#else
#define LOG_DEBUG(str)                do { } while ( false )
#endif

// debugging specific modules
#ifdef DEBUG
#endif

/// calculating histograms
#define HIST_BGR                      0
#define HIST_HS                       1

/// ml
#define LABEL_POSITIVE                1
#define LABEL_NEGATIVE                0

/// tracking
#define TRACKING_CONDENS              0
#define TRACKING_KALMAN               1
#define TRACKING_PARTICLE             2

#define TRACKING_UPDATE_HIST          0
#define TRACKING_UPDATE_HOG           1
#define TRACKING_UPDATE_SIFT          2

#endif // AC_VISION_CONSTANTS_H