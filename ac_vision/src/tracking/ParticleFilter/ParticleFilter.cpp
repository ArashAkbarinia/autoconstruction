#include "ParticleFilter.h"
#include "../../utils/opencv/Hist.h"

#include <iostream>

#define DT  1.0

typedef unsigned int uint;

ParticleFilter::ParticleFilter ( unsigned int num_particles )
  : Condensation ( NUM_STATES, num_particles )
{
  m_mean_confidence = 0.f;
}

ParticleFilter::~ParticleFilter ( void )
{

}

void ParticleFilter::init ( const Rect& selection )
{
  cout << "Initialising particle filter with " << m_num_particles << " particles." << endl;

  // Constant velocity model with constant scale
  m_transition_matrix = ( Mat_<float> ( NUM_STATES, NUM_STATES ) <<
                          1, 0, DT,  0,  0,
                          0, 1,  0, DT,  0,
                          0, 0,  1,  0,  0,
                          0, 0,  0,  1,  0,
                          0, 0,  0,  0,  1 );

  region = selection;
  scale = 1.0;
  const float initial[NUM_STATES] = {selection.x + selection.width / 2.0f, selection.y + selection.height / 2.0f, 0.0f, 0.0f, 1.0f};
  static const float std_dev[NUM_STATES] = { 2.0f,  2.0f,  0.5f,  0.5f,  0.1f};

  init_sample_set ( initial, std_dev );
}

/**
 * Update filter with measurements and time step.
 */
Mat& ParticleFilter::update ( Mat& image, const Size& target_size, Mat& target_hist, int hist_type )
{
  Rect bounds ( 0, 0, image.cols, image.rows );

  // Update the confidence for each particle
  for ( uint i = 0; i < m_num_particles; i++ )
    {
      float tmpscale = MAX ( 0.1, m_particles[i] ( STATE_SCALE ) );
      m_particles[i] ( STATE_SCALE ) = tmpscale;
      int width = round ( target_size.width * tmpscale );
      int height = round ( target_size.height * tmpscale );
      int x = round ( m_particles[i] ( STATE_X ) ) - width / 2;
      int y = round ( m_particles[i] ( STATE_Y ) ) - height / 2;

      Rect tmpregion = Rect ( x, y, width, height ) & bounds;
      Mat image_roi ( image, tmpregion );

      m_confidence[i] = calc_likelyhood ( image_roi, target_hist, hist_type );
    }

  // Project the state forward in time
  time_update ( );

  // Update the confidence at the mean state
  scale = MAX ( 0.1, m_state ( STATE_SCALE ) );
  m_state ( STATE_SCALE ) = scale;
  int width = round ( target_size.width * scale );
  int height = round ( target_size.height * scale );
  int x = round ( m_state ( STATE_X ) ) - width / 2;
  int y = round ( m_state ( STATE_Y ) ) - height / 2;

  region = Rect ( x, y, width, height ) & bounds;
  Mat image_roi ( image, region );

  m_mean_confidence = calc_likelyhood ( image_roi, target_hist, hist_type );

  // Redistribute particles to reacquire the target if the mean state moves
  // off screen.  This usually means the target has been lost due to a mismatch
  // between the modelled motion and actual motion.
  if ( !bounds.contains ( Point ( round ( m_state ( STATE_X ) ), round ( m_state ( STATE_Y ) ) ) ) )
    {
      // TODO: if bounds doesnt contain the new state, we have to look into the scale and and confidence.
      // we can probbaly just move it slightly
//       if ( m_mean_confidence > 0.6 )
//         {
//           // TODO: look for smaller scales.
//           Rect enlarged_region = cv_utils.enlarge_window_width ( region, image, 2.5 );
//           static const float lower_bound[NUM_STATES] = { ( float ) enlarged_region.x, ( float ) enlarged_region.y, -0.5f, -0.5f, 0.9f};
//           static const float upper_bound[NUM_STATES] = { ( float ) enlarged_region.br().x, ( float ) enlarged_region.br().y, 0.5f, 0.5f, 1.1f};
// 
//           cout << "locally: " << m_state << " " << m_mean_confidence << endl;
//           redistribute ( lower_bound, upper_bound );
//         }
//       else
//         {
          static const float lower_bound[NUM_STATES] = {0.0f, 0.0f, -0.5f, -0.5f, 1.0f};
          static const float upper_bound[NUM_STATES] = { ( float ) image.cols, ( float ) image.rows, 0.5f, 0.5f, 2.0f};

          cout << "Redistribute: " << m_state << " " << m_mean_confidence << endl;
          redistribute ( lower_bound, upper_bound );
//         }
    }

  // TODO: put it in a better place, to get rid of big rectangles.
//   if ( region.width > 200 || region.height > 200 )
//     {
//       m_mean_confidence = 0.0;
//     }

  return m_state;
}

// Calculate the likelyhood for a particular region
float ParticleFilter::calc_likelyhood ( Mat& image_roi, Mat& target_hist, int hist_type )
{
  static Mat hist;

  calc_hist ( image_roi, hist, hist_type );

  // TODO: check other types of histogram comparison, e.g. Diffusion Distance for Histogram Comparison
  float bc = compareHist ( target_hist, hist, CV_COMP_BHATTACHARYYA ); // CV_COMP_CORREL

  static const float LAMBDA = 20.f;
  float prob = 0.f;
  if ( bc != 1.f ) // Clamp total mismatch to 0 likelyhood
    {
      prob = exp ( -LAMBDA * ( bc * bc ) );
    }

  return prob;
}

void ParticleFilter::redistribute ( const float lbound[], const float ubound[] )
{
  for ( uint i = 0; i < m_num_particles; i++ )
    {
      for ( uint j = 0; j < m_num_states; j++ )
        {
          float r = m_rng.uniform ( lbound[j], ubound[j] );
          m_particles[i] ( j ) = r;
        }
      m_confidence[i] = 1.0 / ( float ) m_num_particles;
    }
}

Rect ParticleFilter::get_estimated_state ( void )
{
  return region;
}

float ParticleFilter::get_estimated_scale ( void )
{
  return scale;
}

void ParticleFilter::draw_particles ( Mat& image, const Size& target_size, const Scalar& color ) const
{
  Rect bounds ( 0, 0, image.cols, image.rows );

  for ( uint i = 0; i < m_num_particles; i++ )
    {
      int width = round ( target_size.width * m_particles[i] ( STATE_SCALE ) );
      int height = round ( target_size.height * m_particles[i] ( STATE_SCALE ) );
      int x = round ( m_particles[i] ( STATE_X ) ) - width / 2;
      int y = round ( m_particles[i] ( STATE_Y ) ) - height / 2;
      Rect rect = Rect ( x, y, width, height ) & bounds;
      rectangle ( image, rect, color, 1 );
    }
}
