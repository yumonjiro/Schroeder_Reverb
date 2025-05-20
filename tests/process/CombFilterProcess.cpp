// main.cpp (dr_wav を使用)
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>  // C++11以降の乱数生成
#include <ranges>
#include <string>  // std::string を使うため
#include <vector>

#include "CombFilter.h"

// dr_wav.h をインクルードする前に、実装を有効にするためのマクロを定義
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"  // ダウンロードした dr_wav.h をインクルード

// M_PIが未定義の場合の対策
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ホワイトノイズを生成する関数 (-1.0 to 1.0 の範囲)
std::vector<float> generateWhiteNoise(int numSamples) {
  std::vector<float> signal(numSamples);

  // 乱数生成器の準備
  std::random_device rd;   // 非決定的な乱数シードを取得
  std::mt19937 gen(rd());  // メルセンヌ・ツイスター法による乱数生成エンジン
  // -1.0から1.0までの一様実数分布
  std::uniform_real_distribution<float> distrib(-1.0f, 1.0f);

  for (int i = 0; i < numSamples; ++i) {
    signal[i] = distrib(gen);
  }
  return signal;
}
// (generateImpulse, generateSineWave 関数は変更なしなので省略)
std::vector<float> generateImpulse(int numSamples) {
  std::vector<float> signal(numSamples, 0.0f);
  if (numSamples > 0) {
    signal[0] = 1.0f;
  }
  return signal;
}

std::vector<float> generateSineWave(int numSamples, float sampleRate,
                                    float frequency, float amplitude) {
  std::vector<float> signal(numSamples);
  for (int i = 0; i < numSamples; ++i) {
    signal[i] = amplitude * std::sin(2.0f * M_PI * frequency *
                                     static_cast<float>(i) / sampleRate);
  }
  return signal;
}
std::vector<float> generateSinePluck(int numSamples, float sampleRate,
                                     float frequency, float amplitude) {
  std::vector<float> signal(numSamples);
  for (int i = 0; i < numSamples; ++i) {
    signal[i] =
        (((float)(numSamples - i)) / ((float)numSamples)) * amplitude *
        std::sin(2.0f * M_PI * frequency * static_cast<float>(i) / sampleRate);
  }
  return signal;
}

// floatのサンプルデータを16bit整数のWAVファイルに書き出す関数
void writeWavFile(const std::string& filename,
                  const std::vector<float>& audioData,
                  unsigned int sampleRate) {
  drwav_data_format format;
  format.container = drwav_container_riff;  // riffは標準的なWAV
  format.format = DR_WAVE_FORMAT_PCM;       // PCMフォーマット
  format.channels = 1;                      // モノラル
  format.sampleRate = sampleRate;
  format.bitsPerSample = 16;  // 16ビット

  drwav wav;
  if (!drwav_init_file_write(&wav, filename.c_str(), &format, NULL)) {
    std::cerr << "Error: Failed to initialize WAV file for writing: "
              << filename << std::endl;
    return;
  }

  // floatデータを16bit整数に変換
  std::vector<int16_t> pcmData(audioData.size());
  for (size_t i = 0; i < audioData.size(); ++i) {
    // float (-1.0 to 1.0) を int16_t (-32768 to 32767) に変換
    // クリッピングも考慮
    float sample = audioData[i];
    if (sample > 1.0f) sample = 1.0f;
    if (sample < -1.0f) sample = -1.0f;
    pcmData[i] = static_cast<int16_t>(sample * 32767.0f);
  }

  drwav_uint64 framesWritten =
      drwav_write_pcm_frames(&wav, audioData.size(), pcmData.data());
  if (framesWritten != audioData.size()) {
    std::cerr << "Error: Failed to write all PCM frames to WAV file."
              << std::endl;
  } else {
    std::cout << "Successfully wrote " << framesWritten << " frames to "
              << filename << std::endl;
  }

  drwav_uninit(&wav);
}

int main() {
  float sampleRate = 44100.0f;
  int durationSeconds = 2;  // 少し長めにしてみる
  int numSamples = static_cast<int>(sampleRate * durationSeconds);

  float delayTimeSeconds;
  std::cin >> delayTimeSeconds;
  float feedbackGain = 0.5f;

  float maxDelayTimeForDelayLineSec = 0.1f;
  int maxDelaySamples =
      static_cast<int>(sampleRate * maxDelayTimeForDelayLineSec);
  if (maxDelaySamples <= 0)
    maxDelaySamples = static_cast<int>(sampleRate * 0.1);

  CombFilter combFilter(sampleRate, delayTimeSeconds, feedbackGain);

  std::cout << "Comb Filter Test with WAV output" << std::endl;
  // (中略：パラメータ表示など)
  std::cout << "Sample Rate: " << sampleRate << " Hz" << std::endl;
  std::cout << "Initial Delay Time: " << combFilter.getDelayTime() * 1000.0f
            << " ms" << std::endl;
  std::cout << "Initial Feedback Gain: " << combFilter.getGain() << std::endl;
  std::cout << "Max Delay for DelayLine: "
            << maxDelayTimeForDelayLineSec * 1000.0f << " ms"
            << std::endl;  // 修正: maxDelayTimeForDelayLineSec を使用
  std::cout << "------------------------------------" << std::endl;

  //   auto inputSignalView =
  //       generateWhiteNoise(numSamples) |
  //       std::views::transform([](const float x) { return 0.2 * x; });

  //   std::vector<float> inputSignal(inputSignalView.begin(),
  //                                  inputSignalView.end());
  std::vector<float> inputSignal =
      generateSinePluck(numSamples, 44100, 440, 0.4);

  std::cout << "Using Impulse Signal" << std::endl;
  // float testFrequency = 220.0f;
  // float testAmplitude = 0.5f;
  // std::vector<float> inputSignal = generateSineWave(numSamples, sampleRate,
  // testFrequency, testAmplitude); std::cout << "Using Sine Wave Signal (Freq:
  // " << testFrequency << " Hz, Amp: " << testAmplitude << ")" << std::endl;

  std::vector<float> outputSignal(numSamples);

  std::cout << "\nProcessing..." << std::endl;
  for (int i = 0; i < numSamples; ++i) {
    outputSignal[i] = combFilter.process(inputSignal[i]);
  }
  std::cout << "Processing finished." << std::endl;

  // --- WAVファイルへの書き出し ---
  std::string outputFilename = "output_comb_filter.wav";
  writeWavFile(outputFilename, outputSignal,
               static_cast<unsigned int>(sampleRate));

  // (オプション: 標準出力への一部サンプル表示は残しても良い)
  std::cout << "\nFirst 10 processed samples (output):" << std::endl;
  std::cout << std::fixed << std::setprecision(4);
  for (int i = 0; i < 10 && i < numSamples; ++i) {
    std::cout << "Sample " << i << ": " << outputSignal[i] << std::endl;
  }

  return 0;
}