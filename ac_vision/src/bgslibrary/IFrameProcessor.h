#pragma once

#include <opencv/cv.h>

class IFrameProcessor
{
  public:
  virtual void process(const cv:: Mat &input) = 0;
  virtual ~IFrameProcessor(){}
};