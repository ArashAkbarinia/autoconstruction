#ifndef HIST_H
#define HIST_H

#include "CvUtils.h"

void calc_hist ( const Mat& image, Mat& hist, int type );
float compare_hist ( const Mat& image, const Mat& hist, int type );

#endif // HIST_H
