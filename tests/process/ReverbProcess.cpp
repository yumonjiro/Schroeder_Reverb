#include <cmath>  // sin()とM_PIのために必要
#include <iostream>
#include <string>
#include <vector>

// --- 作成したクラスのヘッダをインクルード ---
#include "AllpassFilter.h"
#include "CombFilter.h"
#include "DelayLine.h"
#include "Reverb.h"

// --- dr_wav ライブラリ ---
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

// M_PIが未定義の場合の対策 (WindowsのMSVCなど)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void write_to_wav(const std::string& filename, const std::vector<float>& data,
                  float sampleRate) {
  drwav_data_format format;
  format.container = drwav_container_riff;
  format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
  format.channels = 1;
  format.sampleRate = (drwav_uint32)sampleRate;
  format.bitsPerSample = 32;

  drwav wav;
  if (!drwav_init_file_write(&wav, filename.c_str(), &format, NULL)) {
    std::cerr << "ERROR: Failed to open file for writing: " << filename
              << std::endl;
    return;
  }

  drwav_uint64 framesWritten =
      drwav_write_pcm_frames(&wav, data.size(), data.data());
  drwav_uninit(&wav);

  if (framesWritten != data.size()) {
    std::cerr << "WARNING: Not all samples were written to file." << std::endl;
  } else {
    std::cout << "Successfully wrote WAV file." << std::endl;
  }
}

int main() {
  // === 1. 設定 ===
  const float sampleRate = 44100.0f;
  const int totalDurationSeconds = 4;  // ファイル全体の長さ
  const std::string outputFilename = "sine_reverb_test.wav";

  // サイン波自体の設定
  const float sineFrequency = 440.0f;      // 440Hz (A4)
  const float sineDurationSeconds = 0.2f;  // 0.2秒間だけ鳴らす
  const float sineAmplitude = 0.5f;        // 音量 (1.0が最大)

  // === 2. リバーブのインスタンス化とパラメータ設定 ===
  std::cout << "Creating Reverb instance..." << std::endl;
  Reverb reverb(sampleRate);

  // Dry/Wetを調整して、原音とリバーブ音をミックスする
  reverb.setWetLevel(0.4f);  // 40% wet
  reverb.setDecay(0.80f);

  std::cout << "Reverb settings: Wet=0.4, Decay=0.80" << std::endl;

  // === 3. サイン波バースト信号の生成 ===
  const int numSamples = static_cast<int>(sampleRate * totalDurationSeconds);
  std::vector<float> inputSignal(numSamples, 0.0f);

  const int sineNumSamples = static_cast<int>(sampleRate * sineDurationSeconds);
  for (int i = 0; i < sineNumSamples; ++i) {
    inputSignal[i] =
        sineAmplitude * std::sin(2.0 * M_PI * sineFrequency * i / sampleRate);
  }
  // 残りの期間は無音

  // === 4. リバーブ処理の実行 ===
  std::cout << "Processing sine wave (" << numSamples << " samples)..."
            << std::endl;
  std::vector<float> outputSignal(numSamples);
  for (int i = 0; i < numSamples; ++i) {
    outputSignal[i] = reverb.process(inputSignal[i]);
  }
  std::cout << "Processing finished." << std::endl;

  // === 5. WAVファイルへの書き出し ===
  std::cout << "Writing to WAV file: " << outputFilename << std::endl;
  write_to_wav(outputFilename, outputSignal, sampleRate);

  std::cout << "You can now open '" << outputFilename << "' in an audio editor."
            << std::endl;

  return 0;
}