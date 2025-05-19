#include "CombFilter.h"

CombFilter::CombFilter(float sampleRate, float delayTime, float gain)
    : delayLine((int)(sampleRate * delayTime), sampleRate * 2),
      currentSampleRate(sampleRate),
      delayTime(delayTime),
      gain(gain) {}

float CombFilter::process(float sample) {
  float delayed_output = delayLine.read();
  float processed = sample + gain * delayed_output;
  delayLine.write(processed);
  return processed;
}

void CombFilter::setup(float sampleRate, float delayTime) {}

void CombFilter::updateParameters(float sampleRate, float delayTime,
                                  float gain) {
  int delaySample = (int)(sampleRate * delayTime);
  delayLine.updateDelaySample(delaySample);
  this->delayTime = delayTime;
  this->gain = gain;
  this->currentSampleRate = sampleRate;
}

void CombFilter::reset() { delayLine.clear(); }