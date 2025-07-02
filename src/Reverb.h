#ifndef REVERB_H
#define REVERB_H

#include <vector>

#include "AllpassFilter.h"
#include "CombFilter.h"

class Reverb {
 public:
  Reverb(float sampleRate);

  float process(float sample);
  void reset();

  void setWetLevel(float level);   // 0.0 (dry) to 1.0 (wet)
  void setDecay(float decay);      // 0.0 (short) to 1.0 (long)
  void setDamping(float damping);  // 0.0 (bright) to 1.0 (dark) - オプション

 private:
  float sampleRate_;

  std::vector<CombFilter> combFilters_;
  std::vector<AllpassFilter> allpassFilters_;

  float wetLevel_;
  float dryLevel_;

  // パラメータ設定用の内部変数
  float decay_;
  float damping_;  // オプション

  // 定数
  const float combMixGain_ = 0.25f;  // コムフィルタ出力のミックスゲイン
};

#endif  // REVERB_H