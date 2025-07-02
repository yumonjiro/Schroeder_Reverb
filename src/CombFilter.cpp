#include "CombFilter.h"

CombFilter::CombFilter(float sampleRate, float delayTime, float gain,
                       float dumping)
    : delayLine((int)(sampleRate * delayTime), sampleRate * 2),
      currentSampleRate(sampleRate),
      delayTime(delayTime),
      gain(gain),
      dumping_(dumping) {}

float CombFilter::process(float sample) {
  float delayed_output = delayLine.read();
  // ★ダンピング処理（ローパスフィルタ）
  store_ = delayed_output * (1.0f - dumping_) + store_ * dumping_;

  float processed = sample + gain * store_;
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

void CombFilter::setSampleRate(float newSampleRate) {
  this->currentSampleRate = newSampleRate;
}

void CombFilter::updateDumping(float newDumping) {
  this->dumping_ = newDumping;
}
void CombFilter::reset() { delayLine.clear(); }