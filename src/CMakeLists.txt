cmake_minimum_required(VERSION 3.30)
project(MonicaImageProcess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(DEPLOY_MODE "Build for release deployment" OFF)

message(STATUS "DEPLOY_MODE = ${DEPLOY_MODE}")
message(STATUS "OS = ${CMAKE_SYSTEM_NAME}, Arch = ${CMAKE_HOST_SYSTEM_PROCESSOR}")

set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty")

if(DEPLOY_MODE)
    set(OpenCV_DIR "${THIRDPARTY_DIR}/opencv-install/lib/cmake/opencv4")
endif()

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# JNI
set(JAVA_AWT_LIBRARY NotNeeded)
set(JAVA_JVM_LIBRARY NotNeeded)
set(JAVA_INCLUDE_PATH2 NotNeeded)
set(JAVA_AWT_INCLUDE_PATH NotNeeded)
find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/jni)

# LibRaw
if (DEPLOY_MODE)
    set(LIBRAW_INCLUDE_DIR "${THIRDPARTY_DIR}/libraw/include")
    set(LIBRAW_LIB_PATH    "${THIRDPARTY_DIR}/libraw/lib/libraw.a")
    include_directories(${LIBRAW_INCLUDE_DIR})
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBRAW REQUIRED libraw)
    include_directories(${LIBRAW_INCLUDE_DIRS})
    link_directories(${LIBRAW_LIBRARY_DIRS})
endif()

# libheif
if (DEPLOY_MODE)
    set(LIBHEIF_DIR "${THIRDPARTY_DIR}/heif-suite")
    set(LIBHEIF_INCLUDE_DIR "${LIBHEIF_DIR}/include")
    include_directories(${LIBHEIF_INCLUDE_DIR})
    include_directories(${LIBHEIF_INCLUDE_DIR}/libheif)

    set(LIBHEIF_LIB_PATH "${LIBHEIF_DIR}/lib/libheif.a")
    set(LIBHEIF_DEPENDENCIES
            "${LIBHEIF_DIR}/lib/libde265.a"
            "${LIBHEIF_DIR}/lib/libdav1d.a"
            "${LIBHEIF_DIR}/lib/libaom.a"
            "${LIBHEIF_DIR}/lib/libpng.a"
            "${LIBHEIF_DIR}/lib/libz.a"
            )
else()
    pkg_check_modules(LIBHEIF REQUIRED libheif)
    include_directories(${LIBHEIF_INCLUDE_DIRS})
    link_directories(${LIBHEIF_LIBRARY_DIRS})
endif()

# 输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/install/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/install/lib)

# 源文件
set(SRC_FILES
        ${CMAKE_SOURCE_DIR}/include/library.h
        ${CMAKE_CURRENT_SOURCE_DIR}/library.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/jni/cn_netdiscovery_monica_opencv_ImageProcess.h
        ${CMAKE_CURRENT_SOURCE_DIR}/jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/jni/utils/jni_utils.h
        ${CMAKE_CURRENT_SOURCE_DIR}/jni/utils/jni_utils.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/jni/color_correction/color_correction_internal.h
        ${CMAKE_CURRENT_SOURCE_DIR}/jni/color_correction/jni_color_correction.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/jni/format_decoder/format_decoder_internal.h
        ${CMAKE_CURRENT_SOURCE_DIR}/jni/format_decoder/jni_format_decoder.cpp

        ${CMAKE_SOURCE_DIR}/include/colorcorrection/ColorCorrection.h
        ${CMAKE_CURRENT_SOURCE_DIR}/colorcorrection/ColorCorrection.cpp

        ${CMAKE_SOURCE_DIR}/include/matchTemplate/MatchTemplate.h
        ${CMAKE_CURRENT_SOURCE_DIR}/matchTemplate/MatchTemplate.cpp

        ${CMAKE_SOURCE_DIR}/include/utils/Utils.h
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/Utils.cpp
        )

# 添加库目标
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

# macOS 修复 RPATH
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(${PROJECT_NAME} PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "@loader_path"
            MACOSX_RPATH ON
            )
endif()

# 链接依赖
if (DEPLOY_MODE)
    target_link_libraries(${PROJECT_NAME}
            ${OpenCV_LIBS}
            ${LIBRAW_LIB_PATH}
            ${LIBHEIF_LIB_PATH}
            ${LIBHEIF_DEPENDENCIES}
            z
            stdc++
            )
else()
    target_link_libraries(${PROJECT_NAME}
            ${OpenCV_LIBS}
            ${LIBRAW_LIBRARIES}
            ${LIBHEIF_LIBRARIES}
            )
endif()

# 后处理 fix_dylib
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND DEPLOY_MODE)
    add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND bash ${CMAKE_SOURCE_DIR}/script/fix_dylib_dependencies.sh
            COMMENT "🔧 Fixing dylib dependencies..."
    )
endif()