cmake_minimum_required(VERSION 4.0)

project(Schroeder_Reverb VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


enable_testing()
set(BUILD_TESTING ON)
include(FetchContent)

FetchContent_Declare(
  googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG    release-1.12.1
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(src)

if(BUILD_TESTING)
	add_subdirectory(tests)
endif()

