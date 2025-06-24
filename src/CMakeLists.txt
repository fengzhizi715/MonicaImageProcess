cmake_minimum_required(VERSION 3.30)
project(MonicaImageProcess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 是否是发布模式（默认否）
option(DEPLOY_MODE "Build in deploy (release) mode" OFF)

message(STATUS "===============================")
message(STATUS "DEPLOY_MODE = ${DEPLOY_MODE}")
message(STATUS "Operation system: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Host processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

# 平台判定
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

# ==== ✅ JNI 配置 ====
set(JAVA_AWT_LIBRARY NotNeeded)
set(JAVA_JVM_LIBRARY NotNeeded)
set(JAVA_INCLUDE_PATH2 NotNeeded)
set(JAVA_AWT_INCLUDE_PATH NotNeeded)
find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})
include_directories(jni)

# ==== ✅ LibRaw 配置 ====
if (DEPLOY_MODE)
    set(LIBRAW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/libraw/include)
    set(LIBRAW_LIB_PATH ${CMAKE_SOURCE_DIR}/thirdparty/libraw/lib/libraw.a)
    include_directories(${LIBRAW_INCLUDE_DIR})
else()
    pkg_check_modules(LIBRAW REQUIRED libraw)
    include_directories(${LIBRAW_INCLUDE_DIRS})
    link_directories(${LIBRAW_LIBRARY_DIRS})
endif()

# ==== ✅ libheif 配置 ====
if (DEPLOY_MODE)
    # 自定义构建的 libheif 路径
    set(LIBHEIF_CUSTOM_DIR "$ENV{HOME}/Libs/heif-suite")

    # 设置包含路径
    set(LIBHEIF_INCLUDE_DIR ${LIBHEIF_CUSTOM_DIR}/include)
    include_directories(${LIBHEIF_INCLUDE_DIR})

    # 设置库路径
    set(LIBHEIF_LIB_PATH ${LIBHEIF_CUSTOM_DIR}/lib/libheif.a)

    # 设置依赖库路径（确保所有依赖都链接）
    set(LIBHEIF_DEPENDENCIES
            ${LIBHEIF_CUSTOM_DIR}/lib/libde265.a
            ${LIBHEIF_CUSTOM_DIR}/lib/libdav1d.a
            ${LIBHEIF_CUSTOM_DIR}/lib/libaom.a
            ${LIBHEIF_CUSTOM_DIR}/lib/libpng16.a
            ${LIBHEIF_CUSTOM_DIR}/lib/libz.a
            )
else()
    pkg_check_modules(LIBHEIF REQUIRED libheif)
    include_directories(${LIBHEIF_INCLUDE_DIRS})
    link_directories(${LIBHEIF_LIBRARY_DIRS})
endif()

# 输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/install/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/install/lib)

# 源文件清单
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

# 添加库
add_library(${LIB_NAME} SHARED ${SRC_FILES})

# 设置 RPATH
if(${CURRENT_SYSTEM} MATCHES "MacOS")
    set_target_properties(${LIB_NAME} PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "@loader_path"
            MACOSX_RPATH ON
            )
endif()

# ==== ✅ 链接库 ====
if (DEPLOY_MODE)
    target_link_libraries(${LIB_NAME}
            ${OpenCV_LIBS}
            ${LIBHEIF_LIB_PATH}     # 自定义 libheif
            ${LIBHEIF_DEPENDENCIES} # libheif 的所有依赖
            ${LIBRAW_LIB_PATH}      # 自定义 libraw
            z                 # ✅ 静态链接 libraw 时手动加 zlib
            )
else()
    target_link_libraries(${LIB_NAME}
            ${OpenCV_LIBS}
            ${LIBHEIF_LIBRARIES}
            ${LIBRAW_LIBRARIES}
            )
endif()

# ==== ✅ 后处理 ====
if (${CURRENT_SYSTEM} MATCHES "MacOS")
    add_custom_command(
            TARGET ${LIB_NAME}
            POST_BUILD
            COMMAND bash ${CMAKE_SOURCE_DIR}/script/fix_dylib_dependencies.sh
            COMMENT "🔧 Fixing dylib dependencies..."
    )
endif()