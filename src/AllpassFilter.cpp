#include "AllpassFilter.h"

#include <algorithm>  // std::max, std::min
#include <cmath>      // std::round

// コンストラクタ例
AllpassFilter::AllpassFilter(float initialSampleRate, float initialDelayTime,
                             float initialGain, int maxDelaySamplesForDelayLine)
    : delayLine_(maxDelaySamplesForDelayLine),
      currentSampleRate_(initialSampleRate),
      delayTime_(0.0f),
      gain_(0.0f) {
  updateParameters(initialSampleRate, initialDelayTime, initialGain);
  reset();
}

float AllpassFilter::process(float inputXn) {
  float d_out_n = delayLine_.read();

  float yn = -gain_ * inputXn + d_out_n;

  float d_in_n = inputXn + gain_ * d_out_n;
  // (オプション) ディレイラインへの書き込み値をクリップ (発振防止のため)
  // d_in_n = std::max(-1.0f, std::min(1.0f, d_in_n));
  delayLine_.write(d_in_n);

  return yn;
}

void AllpassFilter::reset() { delayLine_.clear(); }

void AllpassFilter::updateParameters(float newSampleRate, float newDelayTime,
                                     float newGain) {
  currentSampleRate_ = newSampleRate;
  delayTime_ = newDelayTime;

  int delaySamples =
      static_cast<int>(std::round(currentSampleRate_ * delayTime_));
  if (delaySamples <= 0) {
    delaySamples = 1;  // 最小遅延
  }
  delayLine_.updateDelaySample(
      delaySamples);  // DelayLineにこのメソッドがあると仮定

  // ゲインgは通常0から1の間 (絶対値が1未満)
  gain_ = std::max(0.0f, std::min(0.99f, newGain));  // 例: 0.0～0.99
  // もしgが負の値も許容するなら、例えば std::max(-0.99f, std::min(0.99f,
  // newGain));
}