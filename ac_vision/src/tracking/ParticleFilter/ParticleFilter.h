#ifndef PARTICLEFILTER_H
#define PARTICLEFILTER_H

#include <opencv/cv.h>
#include "Condensation.h"
#include "../../utils/opencv/CvUtils.h"

class ParticleFilter : private Condensation
{

public:

  enum FilterStates
  {
    STATE_X,
    STATE_Y,
    STATE_X_VEL,
    STATE_Y_VEL,
    STATE_SCALE,
    NUM_STATES
  };

  ParticleFilter ( unsigned int num_particles );
  ~ParticleFilter ( void );

  void init ( const Rect& selection );

  Mat& update ( Mat& image, const Size& target_size, Mat& target_hist, int hist_type );

  void redistribute ( const float lower_bound[], const float upper_bound[] );
  void draw_particles ( Mat& image, const Size& target_size, const Scalar& color ) const;

  Rect get_estimated_state ( void );
  float get_estimated_scale ( void );

  const Mat& state() const
  {
    return m_state;
  }

  float confidence() const
  {
    return m_mean_confidence;
  };

private:

  float calc_likelyhood ( Mat& image_roi, Mat& target_hist, int hist_type );

  float m_mean_confidence;
  Rect region;
  float scale;

  CvUtils cv_utils;

};

#endif // PARTICLEFILTER_H
