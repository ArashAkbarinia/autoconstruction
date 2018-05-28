#ifndef CONDENSATION_H
#define CONDENSATION_H

#include "../../utils/opencv/CvUtils.h"

class Condensation
{

public:

  Condensation ( unsigned int dynam_params, unsigned int num_particles );
  ~Condensation ( void );

  void init_sample_set ( const float initial[], const float std_dev[] );

  void time_update ( void );

protected:

  unsigned int m_num_states;
  Mat_<float> m_transition_matrix;        // Matrix of the linear system
  Mat_<float>  m_state;                   // Vector of current State
  unsigned int m_num_particles;           // Number of the Samples
  vector<Mat_<float> > m_particles;       // Current particle vectors
  vector<float> m_confidence;             // Confidence for each particle vector
  vector<Mat_<float> > m_new_particles;   // New samples
  RNG m_rng;                              // Random generator
  const float* m_std_dev;

};

#endif // CONDENSATION_H
