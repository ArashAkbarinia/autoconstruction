#include "Hist.h"
#include "../../ac_vision_constants.h"

inline void calc_hist_bgr ( const Mat& bgr, Mat& hist )
{
  static const int channels[] = {0, 1, 2};
  static const int b_bins = 8;
  static const int g_bins = 8;
  static const int r_bins = 8;
  static const int hist_size[] = {b_bins, g_bins, r_bins};
  static const float branges[] = {0, 255};
  static const float granges[] = {0, 255};
  static const float rranges[] = {0, 255};
  static const float* ranges[] = {branges, granges, rranges};
  static const Mat mask;
  static const int dims = 3;
  Mat srcs[] = {bgr};

  calcHist ( srcs, sizeof ( srcs ), channels, mask, hist, dims, hist_size, ranges, true, false );
}

inline void calc_hist_hs ( const Mat& bgr, Mat& hist )
{
  Mat hsv;
  cvtColor ( bgr, hsv, CV_BGR2HSV );

  static const int channels[] = {0, 1};
  static const int h_bins = 15;
  static const int s_bins = 16;
  static const int hist_size[] = {h_bins, s_bins};
  static const float hranges[] = {0, 180};
  static const float sranges[] = {0, 256};
  static const float* ranges[] = {hranges, sranges};
  static const Mat mask;
  static const int dims = 2;
  Mat srcs[] = {hsv};

  calcHist ( srcs, sizeof ( srcs ), channels, mask, hist, dims, hist_size, ranges, true, false );
}

void calc_hist ( const Mat& image, Mat& hist, int type )
{
  switch ( type )
    {
    case HIST_BGR:
      calc_hist_bgr ( image, hist );
      break;
    case HIST_HS:
      calc_hist_hs ( image, hist );
      break;
    default:
      break;
    }
  normalize ( hist, hist, 0, 1, NORM_MINMAX, -1, Mat() );
}

float compare_hist ( const Mat& image, const Mat& hist, int type )
{
  Mat target_hist;
  calc_hist ( image, target_hist, type );

  float bc = compareHist ( target_hist, hist, CV_COMP_BHATTACHARYYA );

  static const float LAMBDA = 20.f;
  float prob = 0.f;
  if ( bc != 1.f ) // Clamp total mismatch to 0 likelyhood
    {
      prob = exp ( -LAMBDA * ( bc * bc ) );
    }

  return prob;
}
