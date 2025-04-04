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

if (${CURRENT_SYSTEM} MATCHES "MacOS" AND ${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x86_64")
    # 指定ONNX Runtime的路径
    set(ONNXRUNTIME_ROOT "/Users/Tony/onnxruntime/onnxruntime-osx-x86_64-1.10.0")
    # 包含ONNX Runtime头文件
    include_directories(${ONNXRUNTIME_ROOT}/include/)
    # 查找ONNX Runtime库文件
    find_library(ONNXRUNTIME_LIBS onnx-runtime PATHS ${ONNXRUNTIME_ROOT}/lib)

    add_library(${PROJECT_NAME} SHARED
            ../include/library.h                                library.cpp
            ../include/common/OnnxRuntimeBase.h                 common/OnnxRuntimeBase.cpp
            ../jni/cn_netdiscovery_monica_opencv_ImageProcess.h ../jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp
            ../include/faceDetect/FaceDetect.h                  faceDetect/FaceDetect.cpp
            ../include/sketchDrawing/SketchDrawing.h            sketchDrawing/SketchDrawing.cpp
            ../include/faceSwap/Yolov8Face.h                    faceSwap/Yolov8Face.cpp
            ../include/faceSwap/Face68Landmarks.h               faceSwap/Face68Landmarks.cpp
            ../include/faceSwap/FaceEmbedding.h                 faceSwap/FaceEmbedding.cpp
            ../include/faceSwap/FaceSwap.h                      faceSwap/FaceSwap.cpp
            ../include/faceSwap/FaceEnhance.h                   faceSwap/FaceEnhance.cpp
            ../include/colorcorrection/ColorCorrection.h        colorcorrection/ColorCorrection.cpp
            ../include/matchTemplate/MatchTemplate.h            matchTemplate/MatchTemplate.cpp
            ../include/utils/Timer.h                            utils/Timer.cpp
            ../include/utils/Utils.h                            utils/Utils.cpp
            ../include/utils/JNIUtils.h                         utils/JNIUtils.cpp
            ../include/Constants.h)

    target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
    target_link_libraries(${PROJECT_NAME} ${ONNXRUNTIME_ROOT}/lib/libonnxruntime.1.10.0.dylib)

elseif(${CURRENT_SYSTEM} MATCHES "MacOS" AND ${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "arm64")
    # 指定ONNX Runtime的路径
    set(ONNXRUNTIME_ROOT "/Users/tony/onnxruntime/onnxruntime-osx-arm64-1.10.0")
    # 包含ONNX Runtime头文件
    include_directories(${ONNXRUNTIME_ROOT}/include/)
    # 查找ONNX Runtime库文件
    find_library(ONNXRUNTIME_LIBS onnx-runtime PATHS ${ONNXRUNTIME_ROOT}/lib)

    add_library(MonicaImageProcess_aarch64 SHARED
            ../include/library.h                                library.cpp
            ../include/common/OnnxRuntimeBase.h                 common/OnnxRuntimeBase.cpp
            ../jni/cn_netdiscovery_monica_opencv_ImageProcess.h ../jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp
            ../include/faceDetect/FaceDetect.h                  faceDetect/FaceDetect.cpp
            ../include/sketchDrawing/SketchDrawing.h            sketchDrawing/SketchDrawing.cpp
            ../include/faceSwap/Yolov8Face.h                    faceSwap/Yolov8Face.cpp
            ../include/faceSwap/Face68Landmarks.h               faceSwap/Face68Landmarks.cpp
            ../include/faceSwap/FaceEmbedding.h                 faceSwap/FaceEmbedding.cpp
            ../include/faceSwap/FaceSwap.h                      faceSwap/FaceSwap.cpp
            ../include/faceSwap/FaceEnhance.h                   faceSwap/FaceEnhance.cpp
            ../include/colorcorrection/ColorCorrection.h        colorcorrection/ColorCorrection.cpp
            ../include/matchTemplate/MatchTemplate.h            matchTemplate/MatchTemplate.cpp
            ../include/utils/Timer.h                            utils/Timer.cpp
            ../include/utils/Utils.h                            utils/Utils.cpp
            ../include/utils/JNIUtils.h                         utils/JNIUtils.cpp
            ../include/Constants.h)

    target_link_libraries(MonicaImageProcess_aarch64 ${OpenCV_LIBS})
    target_link_libraries(MonicaImageProcess_aarch64 ${ONNXRUNTIME_ROOT}/lib/libonnxruntime.1.10.0.dylib)
endif ()