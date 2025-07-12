# MonicaImageProcess

# 一. 说明
该项目是为 https://github.com/fengzhizi715/Monica 项目服务的， 使用的 OpenCV C++ 实现的图像处理算法。

项目目录结构

```
MonicaImageProcess/
├── README.md                        # 项目简介
├── LICENSE                          # 项目许可证文件
├── CMakeLists.txt                   # CMake构建脚本（主）
├── include/                         # 项目公共头文件目录
│   ├── colorcorrection/             # 图像调色相关算法模块的头文件目录
│   ├── matchTemplate/               # 模版匹配的头文件目录
│   └── utils/                       # 工具类模块的头文件目录
├── script/                          # 构建脚本
│   ├── build_libheif_deps.sh        # 从源码编译 libheif 及其依赖的库
│   ├── build_opencv_world_4100.sh   # 从源码编译 opencv 库，包含了 contrib
│   └── fix_dylib_dependencies.sh    # macOS 下修复主库引用 opencv 的路径
├── src/                             # 项目主源代码目录
│   └── colorcorrection/             # 图像调色相关算法模块
│       └── ColorCorrection.cpp      # 图像调色相关算法的源文件
│   └── jni/                         # 给 Java/Kotlin 调用的本地算法
│       ├── cn_netdiscovery_monica_opencv_ImageProcess.h   # jni 对应用层暴露接口的头文件
│       └── cn_netdiscovery_monica_opencv_ImageProcess.cpp # jni 对应用层暴露接口的源文件
│   └── matchTemplate/               # 模版匹配模块
│       └── MatchTemplate.cpp        # 模版匹配相关算法的源文件
│   ├── utils/                       # 工具类模块
│   ├── CMakeLists.txt               # 本地算法的构建脚本
│   └── library.cpp                  # 常规使用 OpenCV 图像算法相关的源文件
├── thirdparty/                      # 外部依赖库
│   └── heif-suite/                  # libheif 库，需要基于 build_libheif_deps.sh 脚本编译，不同的芯片架构编译出来不同，所以没有放
│   └── libraw/                      # libraw 库
│   └── opencv-install/              # opencv 库，需要基于 build_opencv_world_4100.sh 脚本编译，不同的芯片架构编译出来不同，所以没有放
└── .gitignore                       # git 忽略和不追踪的文件
```

# 二. 准备工作

## 2.1 macOS

> 开发、调试时使用 brew 安装的库。
> 
> 部署到生产环境时，这些第三方库大多需要从源码自行编译而不是依赖 brew 安装，这样才能保证用户正常安装软件。

1. 安装 OpenCV

```
brew install opencv
```

2. 安装 pkg-config

```
brew install pkg-config
```

3. 安装 libraw

```
brew install libraw
```

4. 安装 libheif

```
brew install libheif
```

## 2.2 Linux
1. 编译/安装 OpenCV

2. 编译/安装 LibRaw

3. 编译/安装 Libheif


## 2.3 Windows
1. 编译 OpenCV

2. 下载 LibRaw 源码

3. 安装 vcpkg

4. 安装 libde265
```
vcpkg install libde265:x64-windows
```

5. 安装其他可选依赖
```
vcpkg install aom:x64-windows       # AV1 解码器
vcpkg install dav1d:x64-windows     # 更快的 AV1 解码器
vcpkg install libpng:x64-windows    # PNG 支持
vcpkg install zlib:x64-windows      # zlib 压缩支持
```

6. 编译 libheif

# 三. 编译方法(macOS)

1. 在项目根目录下
```
mkdir -p build
cd build
```

2. 使用 CMake 配置项目

* 开发/调试
```
cmake ..
```

或者开启 Release 模式编译(因为 CLion 默认使用 Debug 模式编译)

```
cmake -DCMAKE_BUILD_TYPE=Release ..
```

* 生产部署

开启 Release 模式编译，并且把 DEPLOY_MODE 打开
```
cmake -DCMAKE_BUILD_TYPE=Release -DDEPLOY_MODE=ON ..
```

3. 编译项目
```
cmake --build .
```

> Windows 我没有用 CLion 作为 IDE，也没有用 cmake 构建项目，而是使用 Visual Studio 2022。
> 
> Linux 编译的脚本稍后更新

# 四. 模型的部署

通过 http server 部署了几个模型，方便 Monica 通过调用 http 服务即可调用模型。

相关源码地址：
https://github.com/fengzhizi715/MonicaImageProcessHttpServer

# 五. TODO

* 日志框架的集成