#include "Condensation.h"

typedef unsigned int uint;

Condensation::Condensation ( unsigned int num_states, unsigned int num_particles )
  : m_num_states ( num_states ),
    m_transition_matrix ( m_num_states, m_num_states ),
    m_state ( m_num_states, 1 ),
    m_num_particles ( num_particles ),
    m_particles ( num_particles ),
    m_confidence ( num_particles ),
    m_new_particles ( num_particles ),
    m_rng(),
    m_std_dev ( 0 )
{
  const float prob  = 1.0 / num_particles;
  for ( uint i = 0; i < num_particles; i++ )
    {
      m_particles[i] = Mat_<float> ( m_num_states, 1 );
      m_new_particles[i] = Mat_<float> ( m_num_states, 1 );
      m_confidence[i] = prob;
    }
}

Condensation::~Condensation ( void )
{

}

void Condensation::time_update ( void )
{
  float sum = 0;
  vector<float> m_cumulative ( m_num_particles, 1.0 );

  // Calculate the weighted mean of the particles
  Mat_<float> m_temp = Mat_<float>::zeros (  m_num_states, 1 );
  for ( uint i = 0; i < m_num_particles; i++ )
    {
      m_temp += m_particles[i] * m_confidence[i];
      sum += m_confidence[i];
      m_cumulative[i] = sum;
    }

  // Transform the mean state by the dynamics matrix
  m_temp *= 1.f / sum;
  m_state = m_transition_matrix * m_temp;

  float mean_confidence = sum / m_num_particles;

  // Systematic resampling.  A particle is selected
  // repeatedly until it's confidence is less than
  // the expected cumulative confidence for that index.
  for ( uint i = 0; i < m_num_particles; i++ )
    {
      uint j = 0;
      while ( ( m_cumulative[j] <= ( float ) i * mean_confidence ) && ( j < m_num_particles - 1 ) )
        {
          j++;
        }
      m_particles[j].copyTo ( m_new_particles[i] );
    }
  // Since particle 0 always gets chosen by the above, assign the mean state to it
  m_state.copyTo ( m_new_particles[0] );

  // Transform and randomly perturb the new particles
  for ( uint i = 0; i < m_num_particles; i++ )
    {
      for ( uint j = 0; j < m_num_states; j++ )
        {
          m_temp ( j ) = m_rng.gaussian ( m_std_dev[j] );
        }
      m_particles[i] = m_transition_matrix * m_new_particles[i];
      m_particles[i] += m_temp;
    }
}

void Condensation::init_sample_set ( const float initial[], const float std_dev[] )
{
  this->m_std_dev = std_dev;

  for ( uint i = 0; i < m_num_particles; i++ )
    {
      for ( uint j = 0; j < m_num_states; j++ )
        {
          float r = m_rng.gaussian ( m_std_dev[j] );
          m_particles[i] ( j ) = initial[j] + r;
        }
      m_confidence[i] = 1.0 / ( float ) m_num_particles;
    }

  for ( uint j = 0; j < m_num_states; j++ )
    {
      m_state ( j ) = initial[j];
    }
}
