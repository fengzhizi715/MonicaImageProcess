cmake_minimum_required(VERSION 3.30)
project(MonicaImageProcessHttpServer LANGUAGES CXX)

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
# boost
find_package(Boost COMPONENTS url serialization system filesystem program_options thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

if (${CURRENT_SYSTEM} MATCHES "MacOS" AND ${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x86_64")
    # 指定ONNX Runtime的路径
    set(ONNXRUNTIME_ROOT "/Users/Tony/onnxruntime/onnxruntime-osx-x86_64-1.10.0")
    # 包含ONNX Runtime头文件
    include_directories(${ONNXRUNTIME_ROOT}/include/)
    # 查找ONNX Runtime库文件
    find_library(ONNXRUNTIME_LIBS onnx-runtime PATHS ${ONNXRUNTIME_ROOT}/lib)

    add_executable(${PROJECT_NAME} main.cpp
        GlobalResource.h                            GlobalResource.cpp
        HttpUtils.h                                 HttpUtils.cpp
        ../../include/sketchDrawing/SketchDrawing.h ../sketchDrawing/SketchDrawing.cpp
        ../../include/faceDetect/FaceDetect.h       ../faceDetect/FaceDetect.cpp
        ../../include/faceSwap/Yolov8Face.h         ../faceSwap/Yolov8Face.cpp
        ../../include/faceSwap/Face68Landmarks.h    ../faceSwap/Face68Landmarks.cpp
        ../../include/faceSwap/FaceEmbedding.h      ../faceSwap/FaceEmbedding.cpp
        ../../include/faceSwap/FaceSwap.h           ../faceSwap/FaceSwap.cpp
        ../../include/faceSwap/FaceEnhance.h        ../faceSwap/FaceEnhance.cpp
        ../../include/common/OnnxRuntimeBase.h      ../common/OnnxRuntimeBase.cpp
        ../../include/utils/Timer.h                 ../utils/Timer.cpp
        ../../include/utils/Utils.h                 ../utils/Utils.cpp)

    target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${ONNXRUNTIME_ROOT}/lib/libonnxruntime.1.10.0.dylib)

elseif(${CURRENT_SYSTEM} MATCHES "MacOS" AND ${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "arm64")
    # Mac M 芯片打包的应用名称
    set(ARM64_PROJECT_NAME "MonicaImageProcessHttpServerForAARCH64")

    # 指定ONNX Runtime的路径
    set(ONNXRUNTIME_ROOT "/Users/tony/onnxruntime/onnxruntime-osx-arm64-1.10.0")
    # 包含ONNX Runtime头文件
    include_directories(${ONNXRUNTIME_ROOT}/include/)
    # 查找ONNX Runtime库文件
    find_library(ONNXRUNTIME_LIBS onnx-runtime PATHS ${ONNXRUNTIME_ROOT}/lib)

    add_executable(${ARM64_PROJECT_NAME} main.cpp
            GlobalResource.h                            GlobalResource.cpp
            HttpUtils.h                                 HttpUtils.cpp
            ../../include/sketchDrawing/SketchDrawing.h ../sketchDrawing/SketchDrawing.cpp
            ../../include/faceDetect/FaceDetect.h       ../faceDetect/FaceDetect.cpp
            ../../include/faceSwap/Yolov8Face.h         ../faceSwap/Yolov8Face.cpp
            ../../include/faceSwap/Face68Landmarks.h    ../faceSwap/Face68Landmarks.cpp
            ../../include/faceSwap/FaceEmbedding.h      ../faceSwap/FaceEmbedding.cpp
            ../../include/faceSwap/FaceSwap.h           ../faceSwap/FaceSwap.cpp
            ../../include/faceSwap/FaceEnhance.h        ../faceSwap/FaceEnhance.cpp
            ../../include/common/OnnxRuntimeBase.h      ../common/OnnxRuntimeBase.cpp
            ../../include/utils/Timer.h                 ../utils/Timer.cpp
            ../../include/utils/Utils.h                 ../utils/Utils.cpp)

    target_link_libraries(${ARM64_PROJECT_NAME} ${OpenCV_LIBS})
    target_link_libraries(${ARM64_PROJECT_NAME} ${Boost_LIBRARIES})
    target_link_libraries(${ARM64_PROJECT_NAME} ${ONNXRUNTIME_ROOT}/lib/libonnxruntime.1.10.0.dylib)
endif ()