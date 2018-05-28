#include "LBAdaptiveSOM.h"

#include "../../../ac_vision_constants.h"

#include <ros/package.h>

LBAdaptiveSOM::LBAdaptiveSOM() : firstTime(true), showOutput(true), 
  sensitivity(75), trainingSensitivity(245), learningRate(62), trainingLearningRate(255), trainingSteps(55)
{
  std::cout << "LBAdaptiveSOM()" << std::endl;
}

LBAdaptiveSOM::~LBAdaptiveSOM()
{
  std::cout << "~LBAdaptiveSOM()" << std::endl;
}

void LBAdaptiveSOM::process(const cv::Mat &img_input, cv::Mat &img_output)
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

    m_pBGModel = new BGModelSom(w,h);
    m_pBGModel->InitModel(frame);
  }
  
  m_pBGModel->setBGModelParameter(0,sensitivity);
  m_pBGModel->setBGModelParameter(1,trainingSensitivity);
  m_pBGModel->setBGModelParameter(2,learningRate);
  m_pBGModel->setBGModelParameter(3,trainingLearningRate);
  m_pBGModel->setBGModelParameter(5,trainingSteps);

  m_pBGModel->UpdateModel(frame);

  img_foreground = cv::Mat(m_pBGModel->GetFG());
  img_background = cv::Mat(m_pBGModel->GetBG());
    
  if(showOutput)
  {
    cv::imshow("SOM Mask", img_foreground);
    cv::imshow("SOM Model", img_background);
  }

  img_foreground.copyTo(img_output);
  
  delete frame;
  
  firstTime = false;
}

void LBAdaptiveSOM::finish(void)
{
  delete m_pBGModel;
}

void LBAdaptiveSOM::saveConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "LBAdaptiveSOM.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "sensitivity", sensitivity);
  cvWriteInt(fs, "trainingSensitivity", trainingSensitivity);
  cvWriteInt(fs, "learningRate", learningRate);
  cvWriteInt(fs, "trainingLearningRate", trainingLearningRate);
  cvWriteInt(fs, "trainingSteps", trainingSteps);

  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void LBAdaptiveSOM::loadConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "LBAdaptiveSOM.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_READ);
  
  sensitivity          = cvReadIntByName(fs, 0, "sensitivity", 75);
  trainingSensitivity  = cvReadIntByName(fs, 0, "trainingSensitivity", 245);
  learningRate         = cvReadIntByName(fs, 0, "learningRate", 62);
  trainingLearningRate = cvReadIntByName(fs, 0, "trainingLearningRate", 255);
  trainingSteps        = cvReadIntByName(fs, 0, "trainingSteps", 55);

  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
