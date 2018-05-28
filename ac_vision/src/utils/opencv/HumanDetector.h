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

#ifndef HUMANDETECTOR_H
#define HUMANDETECTOR_H

#include "CvUtils.h"

#include "../../ac_vision_constants.h"

#include <opencv2/ml/ml.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>

class HumanDetector
{

public:

  HumanDetector ( void );

  static vector< Rect > get_cascade_detected_humans_full_body ( const Mat& image );
  static vector< Rect > get_cascade_detected_humans_lower_body ( const Mat& image );
  static vector< Rect > get_cascade_detected_humans_upper_body ( const Mat& image );
  static vector< Rect > get_cascade_detected_pedestrains ( const Mat& image );
  static vector< Rect > get_hog_detected_humans ( const Mat& image );
  static vector< Rect > get_latent_svm_detected_humans ( const Mat& image );

  static bool is_person_present ( const Mat& image, const int type );
  static void initialise ( void );

private:

  static vector< Rect > get_cascade_detected_humans ( const Mat& image, CascadeClassifier cascade_classifier, const Size minsize, const Size maxsize );
  static void load_cascade_classifier ( CascadeClassifier& cascade_classifier, const string file_name );
  static void load_cascade_classifiers ( void );
  static void load_classifiers ( void );
  static void load_latent_svm_detector ( void );

  static bool is_person_present_briefsvm ( const Mat& image );
  static bool is_person_present_brisksvm ( const Mat& image );
  static bool is_person_present_cascade ( const Mat& image );
  static bool is_person_present_freaksvm ( const Mat& image );
  static bool is_person_present_hogsvm ( const Mat& image );
  static bool is_person_present_latentsvm ( const Mat& image );
  static bool is_person_present_siftsvm ( const Mat& image );
  static bool is_person_present_surfsvm ( const Mat& image );

  static vector< float > get_upperbody_detector ( void );
  static void visualise_hogdescriptors ( const Mat& image );
  static void visualise_hoggradient ( const Mat& image );

  static bool is_initialised;
  static CvUtils cv_utils;

  static CascadeClassifier cascade_classifier_full;
  static CascadeClassifier cascade_classifier_lower;
  static CascadeClassifier cascade_classifier_pedestrain;
  static CascadeClassifier cascade_classifier_upper;

  static HOGDescriptor hog;

  static LatentSvmDetector latent_svm_detector;

  static CvSVM svmbrief;
  static CvSVM svmbrisk;
  static CvSVM svmfreak;
  static CvSVM svmhog;
  static CvSVM svmsift;
  static CvSVM svmsurf;

  static DenseFeatureDetector detector;
  static DenseFeatureDetector freak_detector;
  static DenseFeatureDetector brief_detector;
  static BriefDescriptorExtractor brief_extractor;
  static BRISK brisk_extractor;
  static FREAK freak_extractor;
  static SiftDescriptorExtractor sift_extractor;
  static SurfDescriptorExtractor surf_extractor;

};

#endif // HUMANDETECTOR_H
