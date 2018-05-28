#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "package_bgs/FrameDifferenceBGS.h"
#include "package_bgs/StaticFrameDifferenceBGS.h"
#include "package_bgs/WeightedMovingMeanBGS.h"
#include "package_bgs/WeightedMovingVarianceBGS.h"
#include "package_bgs/MixtureOfGaussianV1BGS.h"
#include "package_bgs/MixtureOfGaussianV2BGS.h"
#include "package_bgs/AdaptiveBackgroundLearning.h"
#include "package_bgs/GMG.h"

#include "package_bgs/dp/DPAdaptiveMedianBGS.h"
#include "package_bgs/dp/DPGrimsonGMMBGS.h"
#include "package_bgs/dp/DPZivkovicAGMMBGS.h"
#include "package_bgs/dp/DPMeanBGS.h"
#include "package_bgs/dp/DPWrenGABGS.h"
#include "package_bgs/dp/DPPratiMediodBGS.h"
#include "package_bgs/dp/DPEigenbackgroundBGS.h"

#include "package_bgs/tb/T2FGMM_UM.h"
#include "package_bgs/tb/T2FGMM_UV.h"
#include "package_bgs/tb/T2FMRF_UM.h"
#include "package_bgs/tb/T2FMRF_UV.h"
#include "package_bgs/tb/FuzzySugenoIntegral.h"
#include "package_bgs/tb/FuzzyChoquetIntegral.h"

#include "package_bgs/jmo/MultiLayerBGS.h"

#include "package_bgs/pt/PixelBasedAdaptiveSegmenter.h"

#include "package_bgs/lb/LBSimpleGaussian.h"
#include "package_bgs/lb/LBFuzzyGaussian.h"
#include "package_bgs/lb/LBMixtureOfGaussians.h"
#include "package_bgs/lb/LBAdaptiveSOM.h"
#include "package_bgs/lb/LBFuzzyAdaptiveSOM.h"

int main(int argc, char **argv)
{
  CvCapture *capture = 0;
  capture = cvCaptureFromCAM(0); //capture = cvCaptureFromAVI("video.avi");
  if(!capture){
    std::cerr << "Cannot initialize webcam!" << std::endl;
    return 1;
  }
  
  int resize_factor = 50; // 50% of original image
  IplImage *frame_aux = cvQueryFrame(capture);
  IplImage *frame = cvCreateImage(cvSize((int)((frame_aux->width*resize_factor)/100) , (int)((frame_aux->height*resize_factor)/100)), frame_aux->depth, frame_aux->nChannels);
  
  /* Background Subtraction Methods */
  IBGS *bgs;

  /*** Default Package ***/
  bgs = new FrameDifferenceBGS;
  //bgs = new StaticFrameDifferenceBGS;
  //bgs = new WeightedMovingMeanBGS;
  //bgs = new WeightedMovingVarianceBGS;
  //bgs = new MixtureOfGaussianV1BGS;
  //bgs = new MixtureOfGaussianV2BGS;
  //bgs = new AdaptiveBackgroundLearning;
  //bgs = new GMG;
  
  /*** DP Package (adapted from Donovan Parks) ***/
  //bgs = new DPAdaptiveMedianBGS;
  //bgs = new DPGrimsonGMMBGS;
  //bgs = new DPZivkovicAGMMBGS;
  //bgs = new DPMeanBGS;
  //bgs = new DPWrenGABGS;
  //bgs = new DPPratiMediodBGS;
  //bgs = new DPEigenbackgroundBGS;

  /*** TB Package (adapted from Thierry Bouwmans) ***/
  //bgs = new T2FGMM_UM;
  //bgs = new T2FGMM_UV;
  //bgs = new T2FMRF_UM;
  //bgs = new T2FMRF_UV;
  //bgs = new FuzzySugenoIntegral;
  //bgs = new FuzzyChoquetIntegral;

  /*** JMO Package (adapted from Jean-Marc Odobez) ***/
  //bgs = new MultiLayerBGS;

  /*** PT Package (adapted from Hofmann) ***/
  //bgs = new PixelBasedAdaptiveSegmenter;

  /*** LB Package (adapted from Laurence Bender) ***/
  //bgs = new LBSimpleGaussian;
  //bgs = new LBFuzzyGaussian;
  //bgs = new LBMixtureOfGaussians;
  //bgs = new LBAdaptiveSOM;
  //bgs = new LBFuzzyAdaptiveSOM;

  int key = 0;
  while(key != 'q')
  {
    frame_aux = cvQueryFrame(capture);
    if(!frame_aux) break;

    cvResize(frame_aux, frame);

    cv::Mat img_input(frame);
    cv::imshow("input", img_input);

    // bgs->process(...) method internally shows the foreground mask image
    cv::Mat img_mask;
    bgs->process(img_input, img_mask);
    
    //if(!img_mask.empty())
    //  do something
    
    key = cvWaitKey(1);
  }

  delete bgs;

  cvDestroyAllWindows();
  cvReleaseCapture(&capture);
  
  return 0;
}
