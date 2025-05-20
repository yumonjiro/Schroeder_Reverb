#pragma once
#include "DelayLine.h"

// 　コムフィルタの実装
// 入力 X(n) ---->(+)-------------------> 出力 Y(n)
//               ^  |
//               |  |
//               |  +---->[Delay D]----+
//               |                     |
//               +----<----[Gain g]<---+

class CombFilter {
 public:
  CombFilter(float sampleRate, float delayTime, float gain);

  void setup(float sampleRate, float delayTime);
  float process(float sample);
  void reset();
  void updateParameters(float sampleRate, float delayTime, float gain);

  float getDelayTime() { return delayTime; }
  float getGain() { return gain; }

 private:
  DelayLine delayLine;
  float currentSampleRate;
  float delayTime;
  float gain;
};