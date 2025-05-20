FetchContent_Declare(
    dr_lib
    GIT_REPOSITORY https://github.com/mackron/dr_libs.git
)
FetchContent_MakeAvailable(dr_lib)
add_executable(CombFilterProcess
    CombFilterProcess.cpp
)


# dr_libsをインクルードできるようにする。(このプログラムをコンパイルする用なのでPRIVATEで良い)
target_include_directories(
    CombFilterProcess
    PRIVATE
    ${dr_lib_SOURCE_DIR}
)
target_link_libraries(CombFilterProcess
    PRIVATE 
    CombFilter
    delayline
    dr_libs_interface
    )