cmake_minimum_required(VERSION 3.30)
project(MonicaImageProcess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 是否是发布模式（默认否）
option(DEPLOY_MODE "Build in deploy (release) mode" OFF)

message(STATUS "===============================")
message(STATUS "current operation system is ${CMAKE_SYSTEM}")
message(STATUS "current operation system name is ${CMAKE_SYSTEM_NAME}")

#根据不同平台给CURRENT_SYSTEM命名
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CURRENT_SYSTEM "Linux")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CURRENT_SYSTEM "Windows")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CURRENT_SYSTEM "MacOS")
endif()

find_package(PkgConfig REQUIRED)



# ==== ✅ OpenCV 配置 ====
if (DEPLOY_MODE)
    set(OpenCV_DIR "$ENV{HOME}/Libs/opencv-install/lib/cmake/opencv4")
    find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)
else()
    find_package(OpenCV REQUIRED)
endif()
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
pkg_check_modules(LIBRAW REQUIRED libraw)
include_directories(${LIBRAW_INCLUDE_DIRS})
link_directories(${LIBRAW_LIBRARY_DIRS})

# 添加 libheif 的配置
pkg_check_modules(LIBHEIF REQUIRED libheif)
include_directories(${LIBHEIF_INCLUDE_DIRS})
link_directories(${LIBHEIF_LIBRARY_DIRS})

# 输出路径设定
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/install/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/install/lib)  # 确保可执行文件也输出到此目录

# 定义源文件清单
set(SRC_FILES
        ../include/library.h                                library.cpp
        ../jni/cn_netdiscovery_monica_opencv_ImageProcess.h ../jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp
        ../include/colorcorrection/ColorCorrection.h        colorcorrection/ColorCorrection.cpp
        ../include/matchTemplate/MatchTemplate.h            matchTemplate/MatchTemplate.cpp
        ../include/utils/JNIUtils.h                         utils/JNIUtils.cpp
        ../include/utils/Utils.h                            utils/Utils.cpp
        )

# 生成共享库名
if (${CURRENT_SYSTEM} MATCHES "MacOS" AND ${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x86_64")
    set(LIB_NAME MonicaImageProcess)
elseif (${CURRENT_SYSTEM} MATCHES "MacOS" AND ${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "arm64")
    set(LIB_NAME MonicaImageProcess_aarch64)
elseif (${CURRENT_SYSTEM} MATCHES "Linux")
    set(LIB_NAME MonicaImageProcess)
endif()

add_library(${LIB_NAME} SHARED ${SRC_FILES})

# 设置目标属性 - macOS 设置 RPATH
if(${CURRENT_SYSTEM} MATCHES "MacOS")
    set_target_properties(${LIB_NAME} PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "@loader_path"
            MACOSX_RPATH ON
            )
endif()

# 链接库
target_link_libraries(${LIB_NAME}
        ${OpenCV_LIBS}
        ${LIBRAW_LIBRARIES}
        ${LIBHEIF_LIBRARIES}
)

add_custom_command(
        TARGET ${LIB_NAME}
        POST_BUILD
        COMMAND bash ${CMAKE_SOURCE_DIR}/script/fix_dylib_dependencies.sh
        COMMENT "Fixing dylib dependencies..."
)