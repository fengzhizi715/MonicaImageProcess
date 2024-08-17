cmake_minimum_required(VERSION 3.30)
project(MonicaImageProcess)

set(CMAKE_CXX_STANDARD 17)

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

# 指定ONNX Runtime的路径
set(ONNXRUNTIME_ROOT "/Users/Tony/onnxruntime/onnxruntime-osx-x86_64-1.10.0")
# 包含ONNX Runtime头文件
include_directories(${ONNXRUNTIME_ROOT}/include/)
# 查找ONNX Runtime库文件
find_library(ONNXRUNTIME_LIBS onnx-runtime PATHS ${ONNXRUNTIME_ROOT}/lib)

add_library(MonicaImageProcess SHARED
        src/library.cpp include/library.h
        src/common/OnnxRuntimeBase.cpp include/common/OnnxRuntimeBase.h
        jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp jni/cn_netdiscovery_monica_opencv_ImageProcess.h
        src/faceDetect/FaceDetect.cpp include/faceDetect/FaceDetect.h
        src/sketchDrawing/SketchDrawing.cpp include/sketchDrawing/SketchDrawing.h
        include/Constants.h
        src/utils/Timer.cpp include/utils/Timer.h)

target_link_libraries(MonicaImageProcess ${OpenCV_LIBS})
target_link_libraries(MonicaImageProcess ${ONNXRUNTIME_ROOT}/lib/libonnxruntime.1.10.0.dylib)