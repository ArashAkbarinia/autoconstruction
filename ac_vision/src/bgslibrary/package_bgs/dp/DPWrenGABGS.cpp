#include "DPWrenGABGS.h"

#include "../../../ac_vision_constants.h"

#include <ros/package.h>

DPWrenGABGS::DPWrenGABGS() : firstTime(true), frameNumber(0), threshold(12.25f), alpha(0.005f), learningFrames(30), showOutput(true)
{
  std::cout << "DPWrenGABGS()" << std::endl;
}

DPWrenGABGS::~DPWrenGABGS()
{
  std::cout << "~DPWrenGABGS()" << std::endl;
}

void DPWrenGABGS::process(const cv::Mat &img_input, cv::Mat &img_output)
{
  if(img_input.empty())
    return;

  loadConfig();

  if(firstTime)
    saveConfig();

  frame = new IplImage(img_input);
  
  if(firstTime)
    frame_data.ReleaseMemory(false);
  frame_data = frame;

  if(firstTime)
  {
    int width	= img_input.size().width;
    int height = img_input.size().height;

    lowThresholdMask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    lowThresholdMask.Ptr()->origin = IPL_ORIGIN_BL;

    highThresholdMask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    highThresholdMask.Ptr()->origin = IPL_ORIGIN_BL;

    params.SetFrameSize(width, height);
    params.LowThreshold() = threshold; //3.5f*3.5f;
    params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
    params.Alpha() = alpha; //0.005f;
    params.LearningFrames() = learningFrames; //30;

    bgs.Initalize(params);
    bgs.InitModel(frame_data);
  }

  bgs.Subtract(frameNumber, frame_data, lowThresholdMask, highThresholdMask);
  lowThresholdMask.Clear();
  bgs.Update(frameNumber, frame_data, lowThresholdMask);
  
  cv::Mat foreground(highThresholdMask.Ptr());

  if(showOutput)
    cv::imshow("Gaussian Average (Wren)", foreground);
  
  foreground.copyTo(img_output);

  delete frame;
  firstTime = false;
  frameNumber++;
}

void DPWrenGABGS::saveConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "DPWrenGABGS.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_WRITE);

  cvWriteReal(fs, "threshold", threshold);
  cvWriteReal(fs, "alpha", alpha);
  cvWriteInt(fs, "learningFrames", learningFrames);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void DPWrenGABGS::loadConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "DPWrenGABGS.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_READ);
  
  threshold = cvReadRealByName(fs, 0, "threshold", 12.25f);
  alpha = cvReadRealByName(fs, 0, "alpha", 0.005f);
  learningFrames = cvReadIntByName(fs, 0, "learningFrames", 30);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}

