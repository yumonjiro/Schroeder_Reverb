#pragma once

class AudioEffect {
 public:
  ~AudioEffect() = default;
  virtual float process(float sample);
  virtual void reset();
  virtual void setSampleRate(float newSampleRate);
};