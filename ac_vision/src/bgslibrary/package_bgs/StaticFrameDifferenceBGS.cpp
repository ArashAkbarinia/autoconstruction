#include "StaticFrameDifferenceBGS.h"

#include "../../ac_vision_constants.h"

#include <ros/package.h>


StaticFrameDifferenceBGS::StaticFrameDifferenceBGS() : firstTime(true), enableThreshold(true), threshold(15), showOutput(true)
{
  std::cout << "StaticFrameDifferenceBGS()" << std::endl;
}

StaticFrameDifferenceBGS::~StaticFrameDifferenceBGS()
{
  std::cout << "~StaticFrameDifferenceBGS()" << std::endl;
}

void StaticFrameDifferenceBGS::process(const cv::Mat &img_input, cv::Mat &img_output)
{
  if(img_input.empty())
    return;

  if(img_background.empty())
    img_input.copyTo(img_background);
  
  loadConfig();

  if(firstTime)
    saveConfig();

  cv::absdiff(img_input, img_background, img_foreground);

  if(img_foreground.channels() == 3)
    cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

  if(enableThreshold)
    cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

  if(showOutput)
    cv::imshow("Static Frame Difference", img_foreground);

  img_foreground.copyTo(img_output);

  firstTime = false;
}

void StaticFrameDifferenceBGS::saveConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "StaticFrameDifferenceBGS.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "enableThreshold", enableThreshold);
  cvWriteInt(fs, "threshold", threshold);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void StaticFrameDifferenceBGS::loadConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "StaticFrameDifferenceBGS.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_READ);
  
  enableThreshold = cvReadIntByName(fs, 0, "enableThreshold", true);
  threshold = cvReadIntByName(fs, 0, "threshold", 15);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
