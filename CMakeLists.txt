cmake_minimum_required(VERSION 3.25)
project(MonicaImageProcess)

set(CMAKE_CXX_STANDARD 14)

# 添加 OpenCV 的配置
find_package(OpenCV REQUIRED)
# 打印OpenCV版本
MESSAGE("OpenCV version: ${OpenCV_VERSION}")
# 添加include地址
include_directories(${OpenCV_INCLUDE_DIRS})

# 添加 jni 的配置
set(JAVA_AWT_LIBRARY NotNeeded)
set(JAVA_JVM_LIBRARY NotNeeded)
set(JAVA_INCLUDE_PATH2 NotNeeded)
set(JAVA_AWT_INCLUDE_PATH NotNeeded)
find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})
include_directories(jni)


add_library(MonicaImageProcess SHARED library.cpp
        jni/cn_netdiscovery_monica_opencv_ImageProcess.h
        jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp)

target_link_libraries(MonicaImageProcess ${OpenCV_LIBS})

