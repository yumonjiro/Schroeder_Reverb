#include "CombFilter.h"
#include "gtest/gtest.h"

class CombFilterTest : public ::testing::Test {
 protected:
  const float sampleRate = 48000.0f;
  const float delayTime = 0.0005f;  // 10ms
  const float gain = 0.5f;
};

TEST_F(CombFilterTest, Construction) {
  CombFilter filter(sampleRate, delayTime, gain, 0.4f);

  // First output should be input only (delayed output starts at 0)
  float input = 1.0f;
  EXPECT_FLOAT_EQ(filter.process(input), input);
}

TEST_F(CombFilterTest, ProcessWithZeroGain) {
  CombFilter filter(sampleRate, delayTime, 0.0f, 0.4f);

  // With zero gain, should act as pure delay
  float input = 1.0f;
  int delaySamples = (int)(sampleRate * delayTime);

  // First output is just input
  EXPECT_FLOAT_EQ(filter.process(input), input);

  // Following outputs should be 0 until delay time
  for (int i = 0; i < delaySamples; i++) {
    EXPECT_FLOAT_EQ(filter.process(0.0f), 0.0f);
  }

  // After delay, should get original input
  EXPECT_FLOAT_EQ(filter.process(0.0f), 0.0f);
}

TEST_F(CombFilterTest, ProcessWithFeedback) {
  CombFilter filter(sampleRate, delayTime, 0.5f, 0.4f);

  // Input impulse
  float firstOutput = filter.process(1.0f);
  EXPECT_FLOAT_EQ(firstOutput, 1.0f);

  // Zero input, checking feedback
  int delaySamples = (int)(sampleRate * delayTime);
  for (int i = 0; i < delaySamples - 1; i++) {
    EXPECT_FLOAT_EQ(filter.process(0.0f), 0.0f);
  }

  // After delay, should get gain * input
  EXPECT_FLOAT_EQ(filter.process(0.0f), 0.5f);
}

TEST_F(CombFilterTest, UpdateParameters) {
  CombFilter filter(sampleRate, delayTime, gain, 0.4f);

  // Update all parameters
  float newSampleRate = 96000.0f;
  float newDelayTime = 0.02f;
  float newGain = 0.7f;

  filter.updateParameters(newSampleRate, newDelayTime, newGain);

  // Test with impulse
  float input = 1.0f;
  float firstOutput = filter.process(input);
  EXPECT_FLOAT_EQ(firstOutput, input);

  // After new delay time, should get new gain * input
  int newDelaySamples = (int)(newSampleRate * newDelayTime);
  for (int i = 0; i < newDelaySamples - 1; i++) {
    filter.process(0.0f);
  }
  EXPECT_FLOAT_EQ(filter.process(0.0f), newGain);
}

TEST_F(CombFilterTest, EdgeCases) {
  // Very short delay
  CombFilter shortDelay(sampleRate, 1.0f / sampleRate, gain, 0.4f);
  EXPECT_FLOAT_EQ(shortDelay.process(1.0f), 1.0f);
  EXPECT_FLOAT_EQ(shortDelay.process(0.0f), 0.5f);

  // Very small gain
  CombFilter smallGain(sampleRate, delayTime, 0.001f, 0.4f);
  EXPECT_FLOAT_EQ(smallGain.process(1.0f), 1.0f);

  // Very large gain
  CombFilter largeGain(sampleRate, delayTime, 0.999f, 0.4f);
  EXPECT_FLOAT_EQ(largeGain.process(1.0f), 1.0f);
}