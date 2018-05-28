#include "DPPratiMediodBGS.h"

#include "../../../ac_vision_constants.h"

#include <ros/package.h>

DPPratiMediodBGS::DPPratiMediodBGS() : firstTime(true), frameNumber(0), threshold(30), samplingRate(5), historySize(16), weight(5), showOutput(true)
{
  std::cout << "DPPratiMediodBGS()" << std::endl;
}

DPPratiMediodBGS::~DPPratiMediodBGS()
{
  std::cout << "~DPPratiMediodBGS()" << std::endl;
}

void DPPratiMediodBGS::process(const cv::Mat &img_input, cv::Mat &img_output)
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
    params.LowThreshold() = threshold;
    params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
    params.SamplingRate() = samplingRate;
    params.HistorySize() = historySize;
    params.Weight() = weight;

    bgs.Initalize(params);
    bgs.InitModel(frame_data);
  }

  bgs.Subtract(frameNumber, frame_data, lowThresholdMask, highThresholdMask);
  lowThresholdMask.Clear();
  bgs.Update(frameNumber, frame_data, lowThresholdMask);
  
  cv::Mat foreground(highThresholdMask.Ptr());

  if(showOutput)
    cv::imshow("Temporal Median (Cucchiara&Calderara)", foreground);

  foreground.copyTo(img_output);

  delete frame;
  firstTime = false;
  frameNumber++;
}

void DPPratiMediodBGS::saveConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "DPPratiMediodBGS.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "threshold", threshold);
  cvWriteInt(fs, "samplingRate", samplingRate);
  cvWriteInt(fs, "historySize", historySize);
  cvWriteInt(fs, "weight", weight);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void DPPratiMediodBGS::loadConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "DPPratiMediodBGS.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_READ);
  
  threshold = cvReadIntByName(fs, 0, "threshold", 30);
  samplingRate = cvReadIntByName(fs, 0, "samplingRate", 5);
  historySize = cvReadIntByName(fs, 0, "historySize", 16);
  weight = cvReadIntByName(fs, 0, "weight", 5);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
