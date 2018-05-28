#include "LBMixtureOfGaussians.h"

#include "../../../ac_vision_constants.h"

#include <ros/package.h>

LBMixtureOfGaussians::LBMixtureOfGaussians() : firstTime(true), showOutput(true), sensitivity(81), bgThreshold(83), learningRate(59), noiseVariance(206)
{
  std::cout << "LBMixtureOfGaussians()" << std::endl;
}

LBMixtureOfGaussians::~LBMixtureOfGaussians()
{
  std::cout << "~LBMixtureOfGaussians()" << std::endl;
}

void LBMixtureOfGaussians::process(const cv::Mat &img_input, cv::Mat &img_output)
{
  if(img_input.empty())
    return;

  loadConfig();
  
  IplImage *frame = new IplImage(img_input);
  
  if(firstTime)
  {
    saveConfig();

    int w = cvGetSize(frame).width;
    int h = cvGetSize(frame).height;

    m_pBGModel = new BGModelMog(w,h);
    m_pBGModel->InitModel(frame);
  }
  
  m_pBGModel->setBGModelParameter(0,sensitivity);
  m_pBGModel->setBGModelParameter(1,bgThreshold);
  m_pBGModel->setBGModelParameter(2,learningRate);
  m_pBGModel->setBGModelParameter(3,noiseVariance);

  m_pBGModel->UpdateModel(frame);

  img_foreground = cv::Mat(m_pBGModel->GetFG());
  img_background = cv::Mat(m_pBGModel->GetBG());
    
  if(showOutput)
  {
    cv::imshow("MOG Mask", img_foreground);
    cv::imshow("MOG Model", img_background);
  }

  img_foreground.copyTo(img_output);
  
  delete frame;
  
  firstTime = false;
}

void LBMixtureOfGaussians::finish(void)
{
  delete m_pBGModel;
}

void LBMixtureOfGaussians::saveConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "LBMixtureOfGaussians.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "sensitivity", sensitivity);
  cvWriteInt(fs, "bgThreshold", bgThreshold);
  cvWriteInt(fs, "learningRate", learningRate);
  cvWriteInt(fs, "noiseVariance", noiseVariance);
  
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void LBMixtureOfGaussians::loadConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "LBMixtureOfGaussians.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_READ);
  
  sensitivity = cvReadIntByName(fs, 0, "sensitivity", 81);
  bgThreshold = cvReadIntByName(fs, 0, "bgThreshold", 83);
  learningRate = cvReadIntByName(fs, 0, "learningRate", 59);
  noiseVariance = cvReadIntByName(fs, 0, "noiseVariance", 206);
  
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
