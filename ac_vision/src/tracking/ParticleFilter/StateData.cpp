#include "StateData.h"

StateData::StateData ( void )
{
  associated = false;
  initialised = false;
}

StateData::~StateData ( void )
{
  // TODO: do I need to delete particle_filter?
  // delete particle_filter;
}

void StateData::tracking ( void )
{
  associated = false;
  // TODO: fix the size
//   Size target_size ( selection.size().width * filter->get_estimated_scale(), selection.size().height * filter->get_estimated_scale() );
  Size target_size = selection.size();
  particle_filter->update ( image, target_size, target_histogram, hist_type );
}

void StateData::draw_particles ( Mat& image, const Scalar& color )
{
  particle_filter->draw_particles ( image, selection.size(), color );
}

void StateData::initialise ( unsigned int num_particles, Mat image, Rect selection, int hist_type )
{
  initialised = true;
  associated = true;

  particle_filter = new ParticleFilter ( num_particles );
  this->image = image;
  this->selection = selection;
  this->hist_type = hist_type;

  this->target = Mat ( this->image, this->selection );
  calc_hist ( target, target_histogram, hist_type );

  particle_filter->init ( this->selection );
}

void StateData::update_target_histogram ( Mat& newimage, Rect new_selection )
{
  selection = new_selection;
  Mat roi ( newimage, selection );
  roi.copyTo ( target );

  calc_hist ( roi, target_histogram, hist_type );
}

double StateData::get_particle_filter_confidence ( void ) const
{
  return particle_filter->confidence();
}

Rect StateData::get_target_position ( void )
{
  return particle_filter->get_estimated_state();
}

bool StateData::is_associated ( void )
{
  return associated;
}

bool StateData::is_initialised ( void )
{
  return initialised;
}

void StateData::set_image ( Mat image )
{
  this->image = image;
}
