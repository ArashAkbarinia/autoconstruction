#pragma once

#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "Config.h"
#include "IFrameProcessor.h"

class VideoCapture
{
private:
  IFrameProcessor* frameProcessor;
  CvCapture* capture;
  IplImage* frame;
  int key;
  int64 start_time;
  int64 delta_time;
  double freq;
  double fps;
  long frameNumber;
  long stopAt;
  bool useCamera;
  int cameraIndex;
  bool useVideo;
  std::string videoFileName;
  int input_resize_percent;
  bool showOutput;
  bool enableFlip;

public:
  VideoCapture();
  ~VideoCapture();

  void setFrameProcessor(IFrameProcessor* frameProcessorPtr);
  void setCamera(int cameraIndex);
  void setVideo(std::string filename);
  void start();

private:
  void setUpCamera();
  void setUpVideo();

  void saveConfig();
  void loadConfig();
};

