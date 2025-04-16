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


# 定义一个开关变量用于控制是否启用 CUDA
set(ENABLE_CUDA OFF)
# 针对不同平台进行判断
if(APPLE)
    message(STATUS "当前平台：macOS（不支持 CUDA，使用 CPU 版本）")
    set(ENABLE_CUDA OFF)
elseif(WIN32 OR UNIX)
    # 在 Windows/Linux 上查找 CUDA 工具包（推荐使用 CUDAToolkit 模块，CMake 3.10+ 中包含对 CUDA 的一流支持）
    find_package(CUDAToolkit QUIET)
    if(CUDAToolkit_FOUND)
        message(STATUS "CUDA 工具包已找到，使用 GPU 版本")
        set(ENABLE_CUDA ON)
    else()
        message(STATUS "未找到 CUDA 工具包，使用 CPU 版本")
    endif()
else()
    message(STATUS "未知平台，默认使用 CPU 版本")
    set(ENABLE_CUDA OFF)
endif()


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
        ../../include/cartoon/AnimeGAN.h            ../cartoon/AnimeGAN.cpp
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
            ../../include/cartoon/AnimeGAN.h            ../cartoon/AnimeGAN.cpp
            ../../include/common/OnnxRuntimeBase.h      ../common/OnnxRuntimeBase.cpp
            ../../include/utils/Timer.h                 ../utils/Timer.cpp
            ../../include/utils/Utils.h                 ../utils/Utils.cpp)

    target_link_libraries(${ARM64_PROJECT_NAME} ${OpenCV_LIBS})
    target_link_libraries(${ARM64_PROJECT_NAME} ${Boost_LIBRARIES})
    target_link_libraries(${ARM64_PROJECT_NAME} ${ONNXRUNTIME_ROOT}/lib/libonnxruntime.1.10.0.dylib)

elseif(${CURRENT_SYSTEM} MATCHES "Linux")
    set(LINUX_PROJECT_NAME "MonicaImageProcessHttpServerForLinux")

    # 指定ONNX Runtime的路径
    if(ENABLE_CUDA)
        set(ONNXRUNTIME_ROOT "/home/xxx/onnxruntime/onnxruntime-linux-x64-gpu-1.10.0")
    else()
        set(ONNXRUNTIME_ROOT "/home/xxx/onnxruntime/onnxruntime-linux-x64-1.10.0")
    endif()

    # 包含ONNX Runtime头文件
    include_directories(${ONNXRUNTIME_ROOT}/include/)
    # 查找ONNX Runtime库文件
    find_library(ONNXRUNTIME_LIBS onnx-runtime PATHS ${ONNXRUNTIME_ROOT}/lib)

    add_executable(${LINUX_PROJECT_NAME} main.cpp
            GlobalResource.h                            GlobalResource.cpp
            HttpUtils.h                                 HttpUtils.cpp
            ../../include/sketchDrawing/SketchDrawing.h ../sketchDrawing/SketchDrawing.cpp
            ../../include/faceDetect/FaceDetect.h       ../faceDetect/FaceDetect.cpp
            ../../include/faceSwap/Yolov8Face.h         ../faceSwap/Yolov8Face.cpp
            ../../include/faceSwap/Face68Landmarks.h    ../faceSwap/Face68Landmarks.cpp
            ../../include/faceSwap/FaceEmbedding.h      ../faceSwap/FaceEmbedding.cpp
            ../../include/faceSwap/FaceSwap.h           ../faceSwap/FaceSwap.cpp
            ../../include/faceSwap/FaceEnhance.h        ../faceSwap/FaceEnhance.cpp
            ../../include/cartoon/AnimeGAN.h            ../cartoon/AnimeGAN.cpp
            ../../include/common/OnnxRuntimeBase.h      ../common/OnnxRuntimeBase.cpp
            ../../include/utils/Timer.h                 ../utils/Timer.cpp
            ../../include/utils/Utils.h                 ../utils/Utils.cpp)

    if(ENABLE_CUDA)
        target_compile_definitions(${LINUX_PROJECT_NAME} PRIVATE USE_GPU)
    endif()

    target_link_libraries(${LINUX_PROJECT_NAME} ${OpenCV_LIBS})
    target_link_libraries(${LINUX_PROJECT_NAME} ${Boost_LIBRARIES})
    target_link_libraries(${LINUX_PROJECT_NAME} ${ONNXRUNTIME_ROOT}/lib/libonnxruntime.so.1.10.0)
endif()