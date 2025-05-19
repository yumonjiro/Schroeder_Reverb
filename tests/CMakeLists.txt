set(CMAKE_BUILD_TYPE Debug)

add_executable(run_tests
  test_Schroeder_Reverb.cpp
  test_DelayLine.cpp
  test_CombFilter.cpp
)
target_link_libraries(run_tests
  PRIVATE
  GTest::gtest
  GTest::gtest_main
  delayline
  Schroeder_Reverb
)
include(GoogleTest)
gtest_discover_tests(run_tests)