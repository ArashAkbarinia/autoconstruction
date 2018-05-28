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

#include "HumanTracker.h"

#include "ParticleFilter/ParticleFilter.h"

#include "../utils/ApplicationSettings.h"
#include "../utils/StringUtils.h"
#include "../utils/opencv/HumanDetector.h"
#include "../ac_vision_constants.h"

#define PLOT_PARTICLES

HumanTracker::HumanTracker ( void )
{

}

void HumanTracker::initialise ( void )
{
  this->filter_type = ApplicationSettings::filter_type;
  this->update_type = ApplicationSettings::update_type;

  /// particle filter
  this->closeness_threshold = ApplicationSettings::closeness_threshold;
  this->num_particles = ApplicationSettings::num_particles;

  /// condens

}

vector< Rect > HumanTracker::track ( const Mat& image, const vector< SafetyJacket > jackets )
{
  switch ( filter_type )
    {
    case TRACKING_CONDENS:
      track_condens ( image, jackets );
      break;
    case TRACKING_KALMAN:
      break;
    case TRACKING_PARTICLE:
      track_particle ( image, jackets );
      break;
    default:
      break;
    }
//   switch ( update_type )
//     {
//     case TRACKING_UPDATE_HIST:
//       break;
//     case TRACKING_UPDATE_SIFT:
//       break;
//     case TRACKING_UPDATE_HOG:
//       break;
//     default:
//       break;
//     }

  return get_humanrects();
}

void HumanTracker::track_particle ( const Mat& image, const vector< SafetyJacket > jackets )
{
  unsigned int h = 0;
  for ( vector< StateData >::iterator it = state_datas.begin(); it != state_datas.end(); )
    {
      it->set_image ( image );
      it->tracking ( );
      humans[h].add_safety_jacket_detection ( false );
      Rect upperbody = cv_utils.get_upperbody_rect ( it->get_target_position(), image );
      humans[h].add_sift_detection ( HumanDetector::is_person_present ( Mat ( image, upperbody ), ML_SIFT_SVM ) );

      if ( it->get_particle_filter_confidence() <= ApplicationSettings::tracking_histogram_confidence )
        {
          it = state_datas.erase ( it );
          humans.erase ( humans.begin() + h );
          LOG_DEBUG ( "HumanTracker::track_particle: removing particle filter confidence: " << it->get_particle_filter_confidence() );
        }
      else if ( ( !humans[h].does_safety_jacket_agree() ) || ( !humans[h].does_sift_agree() ) )
        {
          it = state_datas.erase ( it );
          humans.erase ( humans.begin() + h );
          LOG_DEBUG ( "HumanTracker::track_particle: removing particle human confidence" );
        }
      else
        {
          humans[h].set_current_boundry ( upperbody );
          h++;
          it++;
        }
    }

  unsigned int lstate_data = state_datas.size();
  for ( unsigned int i = 0; i < jackets.size(); i++ )
    {
      bool is_new = true;
      for ( unsigned int j = 0; j < lstate_data; j++ )
        {
          if ( are_rects_close ( jackets[i].jacket, state_datas[j].get_target_position() ) )
            {
              humans[j].set_safety_jacket_detection ( true );
              is_new = false;
            }
        }
      if ( is_new &&
           HumanDetector::is_person_present ( Mat ( image, jackets[i].human ), ML_SIFT_SVM ) &&
           HumanDetector::is_person_present ( Mat ( image, jackets[i].human ), ML_LATENT_SVM ) )
        {
          StateData state_data;
          state_data.initialise ( num_particles, image, jackets[i].jacket, HIST_HS );
          state_datas.push_back ( state_data );
          humans.push_back ( Human ( jackets[i].human ) );
          LOG_DEBUG ( "HumanTracker::track_particle: adding new track " << jackets[i].jacket );
        }
    }
}

void HumanTracker::track_condens ( const Mat& image, const vector< SafetyJacket > jackets )
{
  LOG_DEBUG ( "HumanTracker::track_condens: humans before " << humans.size() );

  unsigned int h = 0;
  for ( vector< CvConDensation >::iterator it = condenses.begin(); it != condenses.end(); )
    {
      update_condenses ( image, *it, humans[h] );
      Rect target_rect ( it->State[0] - humans[h].get_current_boundry().width * it->State[4] / 2,
                         it->State[1] - humans[h].get_current_boundry().height * it->State[4] / 2,
                         humans[h].get_current_boundry().width * it->State[4],
                         humans[h].get_current_boundry().height * it->State[4] );
      cout << "State: " << it->State[0] << ", " << it->State[1] << ", " << it->State[2] << ", " << it->State[3] << ", " << it->State[4] << endl;
      humans[h].set_current_boundry ( target_rect );
      it++;
      h++;
    }
  for ( unsigned int i = 0; i < jackets.size(); i++ )
    {
      bool is_new = true;
      for ( unsigned int j = 0; j < condenses.size(); j++ )
        {
          Rect target_rect ( condenses[j].State[0] - humans[h].get_current_boundry().width * condenses[j].State[4] / 2,
                             condenses[j].State[1] - humans[h].get_current_boundry().height * condenses[j].State[4] / 2,
                             humans[h].get_current_boundry().width * condenses[j].State[4],
                             humans[h].get_current_boundry().height * condenses[j].State[4] );
          if ( are_rects_close ( jackets[i].jacket, target_rect ) )
            {
              is_new = false;
            }
        }
      if ( is_new )
        {
          CvConDensation condens;
          initialise_condens ( image, jackets[i].jacket, condens );
          condenses.push_back ( condens );
          humans.push_back ( Human ( jackets[i].human ) );
          Mat histogram;
          calc_hist ( Mat ( image, jackets[i].jacket ), histogram, HIST_HS );
          humans.back().histograms.push_back ( histogram );
        }
    }

  LOG_DEBUG ( "HumanTracker::track_condens: humans after " << humans.size() );
}

void HumanTracker::initialise_condens ( const Mat& image, const Rect jacket, CvConDensation& condens ) const
{
  CvConDensation* condensp = cvCreateConDensation ( 5, 5, ApplicationSettings::num_particles );
  condens = *condensp;
  // FIXME: consider the borders of the image
  Point2f centre = cv_utils.get_rect_centre ( jacket );
  float minrange[] = { centre.x - 2.0f, centre.y - 2.0f, -0.5f, -0.5f, 0.9f };
  float maxrange[] = { centre.x + 2.0f, centre.y + 2.0f, +0.5f, +0.5f, 1.1f };
  CvMat lb;
  CvMat ub;
  cvInitMatHeader ( &lb, 5, 1, CV_32FC1, minrange );
  cvInitMatHeader ( &ub, 5, 1, CV_32FC1, maxrange );
  cvConDensInitSampleSet ( &condens, &lb, &ub );

  cout << "DP: " << condens.DP << " MP: " << condens.MP << endl;
  for ( int i = 0; i < condens.DP; i++ )
    {
      for ( int j = 0; j < condens.MP; j++ )
        {
          if ( i == j )
            {
              condens.DynamMatr[i * condens.DP + j] = 1.0;
            }
          else
            {
              condens.DynamMatr[i * condens.DP + j] = 0.0;
            }
        }
    }
  condens.DynamMatr[2] = 1.0;
  condens.DynamMatr[8] = 1.0;
}

void HumanTracker::update_condenses ( const Mat& image, CvConDensation& condens, const Human human ) const
{
  LOG_DEBUG ( "HumanTracker::update_condenses: start" );
#ifdef PLOT_PARTICLES
  Mat display_image = image.clone();
#endif
  Rect imagebound ( 0, 0, image.cols, image.rows );
  for ( int i = 0; i < condens.SamplesNum; i++ )
    {
      Rect target_rect ( condens.flSamples[i][0] - human.get_current_boundry().width * condens.flSamples[i][4] / 2,
                         condens.flSamples[i][1] - human.get_current_boundry().height * condens.flSamples[i][4] / 2,
                         human.get_current_boundry().width * condens.flSamples[i][4],
                         human.get_current_boundry().height * condens.flSamples[i][4] );
      target_rect &= imagebound;
      condens.flConfidence[i] = compare_hist ( Mat ( image, target_rect ), human.histograms[0], HIST_HS );
#ifdef PLOT_PARTICLES
      rectangle ( display_image, target_rect, GREEN );
      cout << "[" << i << "] " << condens.flConfidence[i] << endl;
      putText ( display_image, StringUtils::inttostr ( i ), target_rect.tl(), FONT_HERSHEY_SIMPLEX, 1, RED );
#endif
    }
  cvConDensUpdateByTime ( &condens );
#ifdef PLOT_PARTICLES
  imshow ( "particles", display_image );
#endif
  LOG_DEBUG ( "HumanTracker::update_condenses: finish" );
}

bool HumanTracker::are_rects_close ( const Rect jacket, const Rect human ) const
{
  double area_proportion = MIN ( jacket.area(), human.area() ) / MAX ( jacket.area(), human.area() );
  if ( cv_utils.euclidean_distance ( jacket, human ) < closeness_threshold/* &&
       area_proportion < ApplicationSettings::closeness_area_threshold*/ )
    {
      return true;
    }

  return false;
}

vector< Rect > HumanTracker::get_humanrects ( void ) const
{
  vector< Rect > rects ( humans.size() );
  for ( unsigned int i = 0; i < humans.size(); i++ )
    {
      rects[i] = humans[i].get_current_boundry();
    }
  return rects;
}
