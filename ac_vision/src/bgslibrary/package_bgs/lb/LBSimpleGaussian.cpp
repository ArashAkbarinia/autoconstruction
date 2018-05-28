#include "LBSimpleGaussian.h"

#include "../../../ac_vision_constants.h"

#include <ros/package.h>

LBSimpleGaussian::LBSimpleGaussian() : firstTime(true), showOutput(true), sensitivity(66), noiseVariance(162), learningRate(18)
{
  std::cout << "LBSimpleGaussian()" << std::endl;
}

LBSimpleGaussian::~LBSimpleGaussian()
{
  std::cout << "~LBSimpleGaussian()" << std::endl;
}

void LBSimpleGaussian::process(const cv::Mat &img_input, cv::Mat &img_output)
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

    m_pBGModel = new BGModelGauss(w,h);
    m_pBGModel->InitModel(frame);
  }
  
  m_pBGModel->setBGModelParameter(0,sensitivity);
  m_pBGModel->setBGModelParameter(1,noiseVariance);
  m_pBGModel->setBGModelParameter(2,learningRate);

  m_pBGModel->UpdateModel(frame);

  img_foreground = cv::Mat(m_pBGModel->GetFG());
  img_background = cv::Mat(m_pBGModel->GetBG());
    
  if(showOutput)
  {
    cv::imshow("SG Mask", img_foreground);
    cv::imshow("SG Model", img_background);
  }

  img_foreground.copyTo(img_output);
  
  delete frame;
  
  firstTime = false;
}

void LBSimpleGaussian::finish(void)
{
  delete m_pBGModel;
}

void LBSimpleGaussian::saveConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "LBSimpleGaussian.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "sensitivity", sensitivity);
  cvWriteInt(fs, "noiseVariance", noiseVariance);
  cvWriteInt(fs, "learningRate", learningRate);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void LBSimpleGaussian::loadConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "LBSimpleGaussian.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_READ);
  
  sensitivity = cvReadIntByName(fs, 0, "sensitivity", 66);
  noiseVariance = cvReadIntByName(fs, 0, "noiseVariance", 162);
  learningRate = cvReadIntByName(fs, 0, "learningRate", 18);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
