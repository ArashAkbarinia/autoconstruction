#include "AdaptiveBackgroundLearning.h"

#include "../../ac_vision_constants.h"

#include <ros/package.h>

AdaptiveBackgroundLearning::AdaptiveBackgroundLearning() : firstTime(true), alpha(0.05), limit(-1), counter(0), minVal(0.0), maxVal(1.0), 
  enableThreshold(true), threshold(15), showForeground(true), showBackground(true)
{
  std::cout << "AdaptiveBackgroundLearning()" << std::endl;
}

AdaptiveBackgroundLearning::~AdaptiveBackgroundLearning()
{
  std::cout << "~AdaptiveBackgroundLearning()" << std::endl;
}

void AdaptiveBackgroundLearning::process(const cv::Mat &img_input, cv::Mat &img_output)
{
  if(img_input.empty())
    return;

  loadConfig();

  if(firstTime)
    saveConfig();

  if(img_background.empty())
    img_input.copyTo(img_background);

  cv::Mat img_input_f(img_input.size(), CV_32F);
  img_input.convertTo(img_input_f, CV_32F, 1./255.);

  cv::Mat img_background_f(img_background.size(), CV_32F);
  img_background.convertTo(img_background_f, CV_32F, 1./255.);

  cv::Mat img_diff_f(img_input.size(), CV_32F);
  cv::absdiff(img_input_f, img_background_f, img_diff_f);

  if((limit > 0 && limit < counter) || limit == -1)
  {
    img_background_f = alpha*img_input_f + (1-alpha)*img_background_f;
    
    cv::Mat img_new_background(img_input.size(), CV_8U);
    img_background_f.convertTo(img_new_background, CV_8U, 255.0/(maxVal - minVal), -minVal);
    img_new_background.copyTo(img_background);

    if(limit > 0 && limit < counter)
      counter++;
  }
  
  cv::Mat img_foreground(img_input.size(), CV_8U);
  img_diff_f.convertTo(img_foreground, CV_8U, 255.0/(maxVal - minVal), -minVal);

  if(img_foreground.channels() == 3)
    cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

  if(enableThreshold)
    cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);
  
  if(showForeground)
    cv::imshow("BG Learning FG Mask", img_foreground);

  if(showBackground)
    cv::imshow("BG Learning BG Model", img_background);

  img_foreground.copyTo(img_output);

  firstTime = false;
}

void AdaptiveBackgroundLearning::saveConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "AdaptiveBackgroundLearning.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_WRITE);

  cvWriteReal(fs, "alpha", alpha);
  cvWriteInt(fs, "limit", limit);
  cvWriteInt(fs, "enableThreshold", enableThreshold);
  cvWriteInt(fs, "threshold", threshold);
  cvWriteInt(fs, "showForeground", showForeground);
  cvWriteInt(fs, "showBackground", showBackground);

  cvReleaseFileStorage(&fs);
}

void AdaptiveBackgroundLearning::loadConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "AdaptiveBackgroundLearning.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_READ);
  
  alpha = cvReadRealByName(fs, 0, "alpha", 0.05);
  limit = cvReadIntByName(fs, 0, "limit", -1);
  enableThreshold = cvReadIntByName(fs, 0, "enableThreshold", true);
  threshold = cvReadIntByName(fs, 0, "threshold", 15);
  showForeground = cvReadIntByName(fs, 0, "showForeground", true);
  showBackground = cvReadIntByName(fs, 0, "showBackground", true);

  cvReleaseFileStorage(&fs);
}
