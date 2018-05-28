#include "GMG.h"

#include "../../ac_vision_constants.h"

#include <ros/package.h>

GMG::GMG() : firstTime(true), initializationFrames(20), decisionThreshold(0.7), showOutput(true)
{
  std::cout << "GMG()" << std::endl;

  cv::initModule_video();
  cv::setUseOptimized(true);
  cv::setNumThreads(8);

  fgbg = cv::Algorithm::create<cv::BackgroundSubtractorGMG>("BackgroundSubtractor.GMG");
}

GMG::~GMG()
{
  std::cout << "~GMG()" << std::endl;
}

void GMG::process(const cv::Mat &img_input, cv::Mat &img_output)
{
  if(img_input.empty())
    return;

  loadConfig();

  if(firstTime)
  {
    fgbg->set("initializationFrames", initializationFrames);
    fgbg->set("decisionThreshold", decisionThreshold);

    saveConfig();
  }
  
  if(fgbg.empty())
  {
    std::cerr << "Failed to create BackgroundSubtractor.GMG Algorithm." << std::endl;
    return;
  }

  (*fgbg)(img_input, img_foreground);

  img_input.copyTo(img_segmentation);
  cv::add(img_input, cv::Scalar(100, 100, 0), img_segmentation, img_foreground);

  if(showOutput)
    cv::imshow("GMG (Godbehere-Matsukawa-Goldberg)", img_foreground);

  img_foreground.copyTo(img_output);

  firstTime = false;
}

void GMG::saveConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "GMG.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "initializationFrames", initializationFrames);
  cvWriteReal(fs, "decisionThreshold", decisionThreshold);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void GMG::loadConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "GMG.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_READ);
  
  initializationFrames = cvReadIntByName(fs, 0, "initializationFrames", 20);
  decisionThreshold = cvReadRealByName(fs, 0, "decisionThreshold", 0.7);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);
  
  cvReleaseFileStorage(&fs);
}
