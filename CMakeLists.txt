cmake_minimum_required(VERSION 3.14)
project(TaskTrack)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
include(FetchContent)
FetchContent_Declare(
    tabulate
    GIT_REPOSITORY https://github.com/p-ranav/tabulate.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(tabulate)
add_executable(TaskTrack
    src/main.cpp
    src/FileManager.cpp
    src/System.cpp
    src/TaskManager.cpp
    src/AuthManager.cpp
)
target_include_directories(TaskTrack PRIVATE
    include
    ${tabulate_SOURCE_DIR}/include
)
target_link_libraries(TaskTrack PRIVATE
    tabulate::tabulate
)