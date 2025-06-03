cmake_minimum_required(VERSION 3.30)
project(MonicaImageProcess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "===============================")
MESSAGE(STATUS "current operation system is ${CMAKE_SYSTEM}")
message(STATUS "current operation system name is ${CMAKE_SYSTEM_NAME}")

#根据不同平台给CURRENT_SYSTEM命名
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CURRENT_SYSTEM "Linux")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CURRENT_SYSTEM "Windows")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CURRENT_SYSTEM "MacOS")
endif()

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

# 添加 libraw 的配置
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBRAW REQUIRED libraw)
include_directories(${LIBRAW_INCLUDE_DIRS})
link_directories(${LIBRAW_LIBRARY_DIRS})

if (${CURRENT_SYSTEM} MATCHES "MacOS" AND ${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x86_64")

    add_library(${PROJECT_NAME} SHARED
            ../include/library.h                                library.cpp
            ../jni/cn_netdiscovery_monica_opencv_ImageProcess.h ../jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp
            ../include/colorcorrection/ColorCorrection.h        colorcorrection/ColorCorrection.cpp
            ../include/matchTemplate/MatchTemplate.h            matchTemplate/MatchTemplate.cpp
            ../include/utils/JNIUtils.h                         utils/JNIUtils.cpp
            ../include/utils/Utils.h                            utils/Utils.cpp
            )

    target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
    target_link_libraries(${PROJECT_NAME} ${LIBRAW_LIBRARIES})
elseif(${CURRENT_SYSTEM} MATCHES "MacOS" AND ${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "arm64")

    add_library(MonicaImageProcess_aarch64 SHARED
            ../include/library.h                                library.cpp
            ../jni/cn_netdiscovery_monica_opencv_ImageProcess.h ../jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp
            ../include/colorcorrection/ColorCorrection.h        colorcorrection/ColorCorrection.cpp
            ../include/matchTemplate/MatchTemplate.h            matchTemplate/MatchTemplate.cpp
            ../include/utils/JNIUtils.h                         utils/JNIUtils.cpp
            ../include/utils/Utils.h                            utils/Utils.cpp
            )

    target_link_libraries(MonicaImageProcess_aarch64 ${OpenCV_LIBS})
    target_link_libraries(MonicaImageProcess_aarch64 ${LIBRAW_LIBRARIES})
elseif(${CURRENT_SYSTEM} MATCHES "Linux")

    add_library(${PROJECT_NAME} SHARED
            ../include/library.h                                library.cpp
            ../jni/cn_netdiscovery_monica_opencv_ImageProcess.h ../jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp
            ../include/colorcorrection/ColorCorrection.h        colorcorrection/ColorCorrection.cpp
            ../include/matchTemplate/MatchTemplate.h            matchTemplate/MatchTemplate.cpp
            ../include/utils/JNIUtils.h                         utils/JNIUtils.cpp
            ../include/utils/Utils.h                            utils/Utils.cpp
            )

    target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
    target_link_libraries(${PROJECT_NAME} ${LIBRAW_LIBRARIES})
endif ()