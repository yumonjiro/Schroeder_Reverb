FetchContent_Declare(
  ring-buffer
  GIT_REPOSITORY https://github.com/jpcima/ring-buffer.git)
FetchContent_MakeAvailable(ring-buffer)
add_library(delayline STATIC DelayLine.cpp DelayLine.h)
target_link_libraries(delayline
  PUBLIC
  ring_buffer 
)
target_include_directories(delayline
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(CombFilter STATIC CombFilter.cpp CombFilter.h)
target_link_libraries(CombFilter
  PRIVATE 
    delayline 
)
target_include_directories(CombFilter
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(Schroeder_Reverb STATIC Schroeder_Reverb.cpp )
target_link_libraries(Schroeder_Reverb
  PRIVATE 
    delayline
    CombFilter
)
# Schroeder_Reverb.h のためにインクルードディレクトリを指定
target_include_directories(Schroeder_Reverb
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)