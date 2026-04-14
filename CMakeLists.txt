cmake_minimum_required(VERSION 3.10)
project(projekt2_opencv)

set(CMAKE_CXX_STANDARD 17)

# path to folder build OpenCV
set(OpenCV_DIR "C:/opencv/build")

# find OpenCV
find_package(OpenCV REQUIRED)

# add header paths
include_directories(${OpenCV_INCLUDE_DIRS})

# add source file
add_executable(projekt5 main.cpp utils.cpp histogram.cpp binarization.cpp)

# link libraries OpenCV
target_link_libraries(projekt5 ${OpenCV_LIBS})