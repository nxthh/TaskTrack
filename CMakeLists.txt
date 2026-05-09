cmake_minimum_required(VERSION 3.14)

project(TaskTrack)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows Support (Ensures Windows API functions are available)
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# Fetch Tabulate Library
include(FetchContent)

FetchContent_Declare(
    tabulate
    GIT_REPOSITORY "https://github.com/p-ranav/tabulate.git"
    GIT_TAG "v1.5"
)

# This downloads the library during the configuration step
FetchContent_MakeAvailable(tabulate)

# Executable
add_executable(TaskTrack
    src/main.cpp
    src/System.cpp
    src/AuthManager.cpp
    src/TaskManager.cpp
    src/FileManager.cpp
)

# Include Headers
# We add both your local 'include' and the fetched 'tabulate' include directory
target_include_directories(TaskTrack PRIVATE
    include
    ${tabulate_SOURCE_DIR}/include
)

# Link Libraries
# tabulate is header-only, but linking ensures any required flags are passed
target_link_libraries(TaskTrack PRIVATE
    tabulate
)
