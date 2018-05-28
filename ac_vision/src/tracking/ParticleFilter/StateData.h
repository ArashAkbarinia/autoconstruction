#ifndef STATEDATA_H
#define STATEDATA_H

#include "ParticleFilter.h"
#include "../../utils/opencv/CvUtils.h"
#include "../../utils/opencv/Hist.h"

class StateData
{

public:
  StateData ( void );
  ~StateData ( void );

  void tracking ( void );
  void initialise ( unsigned int num_particles, Mat image, Rect selection, int hist_type );

  Rect get_target_position ( void );
  void update_target_histogram ( Mat& newimage, Rect new_selection );
  double get_particle_filter_confidence ( void ) const;
  void draw_particles ( Mat& image, const Scalar& color );

  bool is_associated ( void );
  bool is_initialised ( void );

  void set_image ( Mat image );

  Mat target;
  Mat target_histogram;

  Rect selection;

private:
  bool associated;
  bool initialised;

  int hist_type;

  Mat image;

  ParticleFilter *particle_filter;

};

#endif // STATEDATA_H
