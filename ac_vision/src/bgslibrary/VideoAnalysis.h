#pragma once

#include <iostream>
#include <sstream>

#include "VideoCapture.h"
#include "FrameProcessor.h"

class VideoAnalysis
{
private:
  VideoCapture* videoCapture;
  FrameProcessor* frameProcessor;
  bool use_file;
  std::string filename;
  bool use_camera;
  int cameraIndex;
  bool use_comp;
  long frameToStop;
  std::string imgref;

public:
  VideoAnalysis();
  ~VideoAnalysis();

  bool setup(int argc, const char **argv);
  void start();
};

