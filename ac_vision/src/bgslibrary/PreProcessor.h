#pragma once

#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

class PreProcessor
{
private:
  bool firstTime;
  bool equalizeHist;
  bool gaussianBlur;
  cv::Mat img_gray;
  bool enableShow;

public:
  PreProcessor();
  ~PreProcessor();

  void setEqualizeHist(bool value);
  void setGaussianBlur(bool value);
  cv::Mat getGrayScale(); 

  void process(const cv::Mat &img_input, cv::Mat &img_output);

  void rotate(const cv::Mat &img_input, cv::Mat &img_output, float angle);
  void applyCanny(const cv::Mat &img_input, cv::Mat &img_output);

private:
  void saveConfig();
  void loadConfig();
};

