#include "Reverb.h"

#include <algorithm>
#include <cmath>
#include <vector>  // vectorをインクルード

// --- コンストラクタ ---
Reverb::Reverb(float sampleRate)
    : sampleRate_(sampleRate), wetLevel_(0.5f), dryLevel_(0.5f), decay_(0.5f) {
  // --- フィルタのパラメータ定義 ---
  // コムフィルタの遅延時間 (秒)
  const std::vector<float> combDelayTimes = {0.0297f, 0.0371f, 0.0411f,
                                             0.0437f};
  // オールパスフィルタの遅延時間 (秒)
  const std::vector<float> allpassDelayTimes = {0.0098f, 0.0031f};
  const float allpassGain = 0.7f;

  // --- コムフィルタのインスタンス化 ---
  // CombFilterの定義に合わせてインスタンスを生成
  for (const float delay : combDelayTimes) {
    // 初期ゲインは0.0にしておき、後でsetDecayで設定する
    combFilters_.emplace_back(sampleRate_, delay, 0.0f, 0.4f);
  }

  // --- オールパスフィルタのインスタンス化 ---
  // AllpassFilterの定義に合わせてインスタンスを生成
  // オールパスフィルタに必要な最大遅延時間を計算（少し余裕を持たせる）
  float maxAllpassDelayTime = 0.0f;
  for (const float delay : allpassDelayTimes) {
    if (delay > maxAllpassDelayTime) {
      maxAllpassDelayTime = delay;
    }
  }
  // 余裕を持って10ms追加 or 1.1倍など
  int maxAllpassSamples =
      static_cast<int>(sampleRate_ * (maxAllpassDelayTime + 0.01f));

  for (const float delay : allpassDelayTimes) {
    allpassFilters_.emplace_back(
        sampleRate_,       // initialSampleRate
        delay,             // initialDelayTime
        allpassGain,       // initialGain
        maxAllpassSamples  // maxDelaySamplesForDelayLine
    );
  }

  // 初期パラメータを適用
  setWetLevel(wetLevel_);
  setDecay(decay_);
  setDamping(0.4f);
  reset();
}

// --- processメソッド (変更なし) ---
float Reverb::process(float sample) {
  float combOutput = 0.0f;
  for (auto& comb : combFilters_) {
    combOutput += comb.process(sample);
  }

  // コムフィルタの出力をミックスするゲインを適用
  // combMixGain_ は Reverb.h で定義された定数 (例: 0.25f)
  float mixedCombOutput = combOutput * combMixGain_;

  float allpassOutput = mixedCombOutput;
  for (auto& ap : allpassFilters_) {
    allpassOutput = ap.process(allpassOutput);
  }

  return (dryLevel_ * sample) + (wetLevel_ * allpassOutput);
}

// --- resetメソッド (変更なし) ---
void Reverb::reset() {
  for (auto& comb : combFilters_) {
    comb.reset();
  }
  for (auto& ap : allpassFilters_) {
    ap.reset();
  }
}

// --- setWetLevelメソッド (変更なし) ---
void Reverb::setWetLevel(float level) {
  level = std::max(0.0f, std::min(1.0f, level));
  wetLevel_ = level;
  dryLevel_ = 1.0f - level;  // 線形ミックス
  // dryLevel_ = sqrt(1.0f - (level * level)); // 等パワーミックス
}

// --- setDecayメソッド (変更なし) ---
void Reverb::setDecay(float decay) {
  decay_ = std::max(0.0f, std::min(1.0f, decay));

  // decay (0.0-1.0) を実際のフィードバックゲインにマッピング
  float baseGain = 0.7f + decay_ * 0.28f;  // 0.7 ~ 0.98 の範囲にマッピング

  // 各コムフィルタのゲインを更新
  std::vector<float> gainMultipliers = {1.01f, 0.98f, 0.96f, 1.02f};
  for (size_t i = 0; i < combFilters_.size(); ++i) {
    if (i < gainMultipliers.size()) {
      float newGain = baseGain * gainMultipliers[i];
      newGain = std::min(0.999f, newGain);

      combFilters_[i].updateParameters(sampleRate_,
                                       combFilters_[i].getDelayTime(), newGain);
    }
  }
}

void Reverb::setDamping(float damping) {
  // damping_ の値を 0.0 から 1.0 の範囲にクリップ
  damping_ = std::max(0.0f, std::min(1.0f, damping));

  // すべてのコムフィルタにダンピング値を設定
  for (auto& comb : combFilters_) {
    // CombFilterにupdateDampingメソッドがあると仮定
    comb.updateDumping(damping_);
  }
}
// ダンピング関連はCombFilterに実装がないためコメントアウト
/*
void Reverb::setDamping(float damping) {
    // ...
}
*/