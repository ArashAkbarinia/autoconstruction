#include "LBFuzzyAdaptiveSOM.h"

#include "../../../ac_vision_constants.h"

#include <ros/package.h>

LBFuzzyAdaptiveSOM::LBFuzzyAdaptiveSOM() : firstTime(true), showOutput(true), 
  sensitivity(90), trainingSensitivity(240), learningRate(38), trainingLearningRate(255), trainingSteps(81)
{
  std::cout << "LBFuzzyAdaptiveSOM()" << std::endl;
}

LBFuzzyAdaptiveSOM::~LBFuzzyAdaptiveSOM()
{
  std::cout << "~LBFuzzyAdaptiveSOM()" << std::endl;
}

void LBFuzzyAdaptiveSOM::process(const cv::Mat &img_input, cv::Mat &img_output)
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

    m_pBGModel = new BGModelFuzzySom(w,h);
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
    cv::imshow("FSOM Mask", img_foreground);
    cv::imshow("FSOM Model", img_background);
  }

  img_foreground.copyTo(img_output);
  
  delete frame;
  
  firstTime = false;
}

void LBFuzzyAdaptiveSOM::finish(void)
{
  delete m_pBGModel;
}

void LBFuzzyAdaptiveSOM::saveConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "LBFuzzyAdaptiveSOM.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "sensitivity", sensitivity);
  cvWriteInt(fs, "trainingSensitivity", trainingSensitivity);
  cvWriteInt(fs, "learningRate", learningRate);
  cvWriteInt(fs, "trainingLearningRate", trainingLearningRate);
  cvWriteInt(fs, "trainingSteps", trainingSteps);

  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void LBFuzzyAdaptiveSOM::loadConfig()
{
  std::string package_path = ros::package::getPath ( AC_VISION_NODE );
  std::string file_storage_path;
  file_storage_path.append ( package_path );
  file_storage_path.append ( BGS_CONFIG );
  file_storage_path.append ( "LBFuzzyAdaptiveSOM.xml" );
  CvFileStorage* fs = cvOpenFileStorage(file_storage_path.c_str(), 0, CV_STORAGE_READ);
  
  sensitivity          = cvReadIntByName(fs, 0, "sensitivity", 90);
  trainingSensitivity  = cvReadIntByName(fs, 0, "trainingSensitivity", 240);
  learningRate         = cvReadIntByName(fs, 0, "learningRate", 38);
  trainingLearningRate = cvReadIntByName(fs, 0, "trainingLearningRate", 255);
  trainingSteps        = cvReadIntByName(fs, 0, "trainingSteps", 81);

  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}