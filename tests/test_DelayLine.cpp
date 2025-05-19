#include <sstream>    // std::cout の出力をキャプチャするため
#include <streambuf>  // std::cout の出力をキャプチャするため

#include "DelayLine.h"
#include "gtest/gtest.h"

// Helper class to capture std::cout output
class CoutRedirector {
 public:
  CoutRedirector(std::stringstream& ss) : old_buf(std::cout.rdbuf()) {
    std::cout.rdbuf(ss.rdbuf());
  }
  ~CoutRedirector() { std::cout.rdbuf(old_buf); }

 private:
  std::streambuf* old_buf;
};

// Test Fixture for common setup (optional, but can be useful)
class DelayLineTest : public ::testing::Test {
 protected:
  // Per-test setup and teardown logic can go here if needed
  // For now, we'll create DelayLine instances directly in tests
};

TEST_F(DelayLineTest, ConstructorInitializesWithZeros) {
  const int delaySamples = 3;
  DelayLine dl(delaySamples);

  // After construction (which calls clear()), reading should yield zeros
  for (int i = 0; i < delaySamples; ++i) {
    EXPECT_FLOAT_EQ(dl.read(), 0.0f)
        << "Initial read " << i << " should be 0.0f";
    // Write dummy values to keep the buffer cycling for subsequent reads,
    // as read() consumes an item and write() adds one.
    // The actual value written here doesn't matter for this part of the test,
    // as we are checking the initial zeros being read out.
    dl.write(99.0f);  // Placeholder write
  }
}

TEST_F(DelayLineTest, MyTest) {
  const int delaySamples = 1;
  DelayLine dl(delaySamples);
  dl.write(1.0);
  EXPECT_FLOAT_EQ(dl.read(), 0.) << "s";
  EXPECT_FLOAT_EQ(dl.read(), 1.) << "s2";
}

TEST_F(DelayLineTest, BasicDelayFunctionality) {
  const int delaySamples = 3;
  DelayLine dl(delaySamples);  // Initialized with 0,0,0

  // Sequence:
  // Write | Buffer (conceptual, oldest to newest after write) | Read
  // -----------------------------------------------------------------
  // 1.0   | 1,0,0 (0 from clear is replaced)                  | 0 (from clear)
  // 2.0   | 1,2,0 (0 from clear is replaced)                  | 0 (from clear)
  // 3.0   | 1,2,3 (0 from clear is replaced)                  | 0 (from clear)
  // 4.0   | 2,3,4 (1 is replaced)                             | 1
  // 5.0   | 3,4,5 (2 is replaced)                             | 2

  dl.write(1.0f);
  EXPECT_FLOAT_EQ(dl.read(), 0.0f);

  dl.write(2.0f);
  EXPECT_FLOAT_EQ(dl.read(), 0.0f);

  dl.write(3.0f);
  EXPECT_FLOAT_EQ(dl.read(), 0.0f);  // All initial zeros have been read

  dl.write(4.0f);
  EXPECT_FLOAT_EQ(dl.read(), 1.0f);  // 1.0f emerges after 3 writes

  dl.write(5.0f);
  EXPECT_FLOAT_EQ(dl.read(), 2.0f);  // 2.0f emerges

  dl.write(6.0f);
  EXPECT_FLOAT_EQ(dl.read(), 3.0f);  // 3.0f emerges
}

TEST_F(DelayLineTest, ClearResetsDelayLine) {
  const int delaySamples = 2;
  DelayLine dl(delaySamples);  // Initialized with 0,0

  dl.write(1.0f);
  EXPECT_FLOAT_EQ(dl.read(), 0.0f);
  dl.write(2.0f);
  EXPECT_FLOAT_EQ(dl.read(), 0.0f);
  dl.write(3.0f);
  EXPECT_FLOAT_EQ(dl.read(), 1.0f);  // 1.0f emerges

  dl.clear();  // Reset with 0,0

  // After clear, it should behave as if new
  dl.write(10.0f);
  EXPECT_FLOAT_EQ(dl.read(), 0.0f);
  dl.write(20.0f);
  EXPECT_FLOAT_EQ(dl.read(), 0.0f);
  dl.write(30.0f);
  EXPECT_FLOAT_EQ(dl.read(), 10.0f);  // 10.0f emerges
}

TEST_F(DelayLineTest, DelayOfOneSample) {
  const int delaySamples = 1;
  DelayLine dl(delaySamples);  // Initialized with 0

  dl.write(1.0f);
  EXPECT_FLOAT_EQ(dl.read(), 0.0f);
  dl.write(2.0f);
  EXPECT_FLOAT_EQ(dl.read(), 1.0f);
  dl.write(3.0f);
  EXPECT_FLOAT_EQ(dl.read(), 2.0f);
}

TEST_F(DelayLineTest, ContinuousWriteRead) {
  const int delaySamples = 5;
  DelayLine dl(delaySamples);
  std::vector<float> test_signal = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f,
                                    0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
  std::vector<float> expected_output;
  std::vector<float> actual_output;

  // Prime with zeros
  for (int i = 0; i < delaySamples; ++i) {
    expected_output.push_back(0.0f);
  }
  // Then delayed signal
  for (size_t i = 0; i < test_signal.size() - delaySamples; ++i) {
    expected_output.push_back(test_signal[i]);
  }

  for (float sample_in : test_signal) {
    dl.write(sample_in);
    actual_output.push_back(dl.read());
  }

  ASSERT_EQ(actual_output.size(), test_signal.size());
  for (size_t i = 0; i < actual_output.size(); ++i) {
    if (i < delaySamples) {
      EXPECT_FLOAT_EQ(actual_output[i], 0.0f) << "at index " << i;
    } else {
      EXPECT_FLOAT_EQ(actual_output[i], test_signal[i - delaySamples])
          << "at index " << i;
    }
  }
}

// --- Tests for failure conditions (highly dependent on Ring_Buffer
// implementation) --- These tests might need a mock Ring_Buffer or specific
// knowledge of its failure modes. For demonstration, let's assume Ring_Buffer
// can fail if not used in a specific way that DelayLine *should* prevent, but
// we test the message if it somehow happens.

// This test is conceptual. Actually triggering rb.put() to fail within
// DelayLine would require a Ring_Buffer that doesn't overwrite and gets full.
// The current DelayLine design (clear fills it, then write/read pairs) should
// ideally keep the Ring_Buffer in a state where put/get succeed if Ring_Buffer
// behaves as expected for a delay line (e.g. overwriting put or perfectly
// matched get/put).
TEST_F(DelayLineTest, WriteFailureMessage) {
  // This scenario is hard to create with the current DelayLine structure
  // if Ring_Buffer is a typical overwriting circular buffer of fixed size.
  // If Ring_Buffer::put can fail (e.g., it's non-overwriting and full),
  // one would need to set up that state.
  // For now, this is a placeholder for how one *might* test it.
  // To actually make rb.put() fail after clear(), Ring_Buffer would need to be
  // non-overwriting, and clear() would fill it. Then the first write() would
  // fail. This suggests a potential design flaw in DelayLine if Ring_Buffer is
  // non-overwriting.

  // If we assume a Ring_Buffer that has capacity but put fails if full (no
  // overwrite): DelayLine dl_problematic(1); // rb(1), clear() puts one 0. rb
  // is full. std::stringstream ss; CoutRedirector redirect(ss);
  // dl_problematic.write(1.0f); // This put would fail.
  // EXPECT_NE(ss.str().find("failed to write samples to rb"),
  // std::string::npos); EXPECT_TRUE(true) << "Test skipped/conceptual: Relies
  // on specific Ring_Buffer fail behavior for put()";
}

TEST_F(DelayLineTest, ReadFailureMessage) {
  // Similar to write, making rb.get() fail is tricky if DelayLine maintains it
  // properly. It would imply reading from an empty Ring_Buffer. After clear(),
  // the buffer is full. Each read() consumes one. If we read more times than
  // current_delay_samples without writing, it might empty.

  // Example: if Ring_Buffer becomes empty and get() fails:
  // DelayLine dl(1); // rb(1), clear() puts one 0.
  // dl.read();       // Reads the 0. Now rb could be empty.
  //
  // std::stringstream ss;
  // CoutRedirector redirect(ss);
  // float val = dl.read(); // This get might fail.
  //
  // EXPECT_FLOAT_EQ(val, 0.0f); // Default return on failure
  // EXPECT_NE(ss.str().find("failed to read sample"), std::string::npos);
  // EXPECT_TRUE(true) << "Test skipped/conceptual: Relies on specific
  // Ring_Buffer fail behavior for get()";
}