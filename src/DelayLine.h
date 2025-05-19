#pragma once
#include <ring_buffer/ring_buffer.h>

#include <exception>
#include <iostream>
#include <memory>
#include <vector>

class DelayLine {
 public:
  // キャパシティは、バイト単位！ delaysample * sizeof(float)
  DelayLine(int delaySamples, int capacity)
      : currentDelaySamples(delaySamples),
        rb(std::make_unique<Ring_Buffer>(capacity * sizeof(float))) {
    clear();
  }
  // キャパシティは、バイト単位！ delaysample * sizeof(float)
  DelayLine(int delaySamples)
      : currentDelaySamples(delaySamples),
        rb(std::make_unique<Ring_Buffer>((delaySamples + 1) * sizeof(float))) {
    clear();
  }

  void write(float sample) {
    bool res = rb->put(sample);
    if (!res) {
      std::cout << "failed to write samples to rb" << std::endl;
    }
  }

  float read() {
    float sample;
    if (rb->get(sample)) {
      return sample;
    } else {
      std::cout << "failed to read sample" << std::endl;
      return 0.;
    }
  }
  void updateDelaySample(int newDelaySamples) {
    if (currentDelaySamples > newDelaySamples) {
      rb->discard((size_t)(currentDelaySamples - newDelaySamples));
    } else {
      std::vector<float> zeros(newDelaySamples - currentDelaySamples, 0.f);
      rb->put(zeros.data(), zeros.size());
    }
    currentDelaySamples = newDelaySamples;
  }
  void clear() {
    if (rb->size_used() > 0) {
      bool res = rb->discard(rb->size_used());
    }
    std::vector<float> zeros(currentDelaySamples, 0.f);

    rb->put(zeros.data(), zeros.size());
  }
  void reset(int newDelaySamples) {
    rb = std::make_unique<Ring_Buffer>((newDelaySamples + 1) * sizeof(float));
  }

 private:
  // 固定長リングバッファ
  std::unique_ptr<Ring_Buffer> rb;
  int currentDelaySamples;
};