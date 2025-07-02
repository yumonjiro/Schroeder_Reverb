#ifndef ALLPASSFILTER_H
#define ALLPASSFILTER_H

#include "DelayLine.h"  // 作成済みのDelayLine

class AllpassFilter {
 public:
  // コンストラクタ (例: 最大遅延長も指定する方が柔軟かも)
  AllpassFilter(float initialSampleRate, float initialDelayTime,
                float initialGain, int maxDelaySamplesForDelayLine);
  // または、CombFilterと同様のインターフェースで
  // AllpassFilter(float sampleRate, float delayTime, float gain);

  float process(float sample);
  void reset();
  void updateParameters(float newSampleRate, float newDelayTime, float newGain);

 private:
  DelayLine delayLine_;
  float currentSampleRate_;
  float delayTime_;
  float gain_;  // Schroeder Allpass の 'g'
};

#endif  // ALLPASSFILTER_H