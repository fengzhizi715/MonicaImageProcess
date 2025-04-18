# MonicaImageProcess

# 一. 说明
该项目是为 https://github.com/fengzhizi715/Monica 项目服务的， 使用的 OpenCV C++ 实现的图像处理算法和调用。

项目目录结构

```
MonicaImageProcess/
├── README.md                        # 项目简介
├── LICENSE                          # 项目许可证文件
├── CMakeLists.txt                   # CMake构建脚本（主）
├── include/                         # 项目公共头文件目录
│   └── colorcorrection/             # 图像调色相关算法模块的头文件目录
│   └── matchTemplate/               # 模版匹配的头文件目录
│   └── utils/                       # 工具类模块的头文件目录
├── jni/                             # 给 Java/Kotlin 调用的本地算法
│       ├── cn_netdiscovery_monica_opencv_ImageProcess.h   # jni 对应用层暴露接口的头文件
│       └── cn_netdiscovery_monica_opencv_ImageProcess.cpp # jni 对应用层暴露接口的源文件
├── src/                             # 项目主源代码目录
│   └── colorcorrection/             # 图像调色相关算法模块
│       └── ColorCorrection.cpp      # 图像调色相关算法的源文件
│   └── matchTemplate/               # 模版匹配模块
│       └── MatchTemplate.cpp        # 模版匹配相关算法的源文件
│   └── utils/                       # 工具类模块
│   └── CMakeLists.txt               # 本地算法的构建脚本
│   └── library.cpp                  # 常规使用 OpenCV 图像算法相关的源文件
└── .gitignore                       # git 忽略和不追踪的文件
```

# 二. 编译方法

1. 进入 src 目录：
```
cd src
```

2. 创建一个单独的构建目录
```
mkdir build
cd build
```

3. 使用 CMake 配置项目：
```
cmake ..
```

4. 编译项目
```
cmake --build .
```

# 三. 模型的部署

通过 http server 部署了几个模型，方便 Monica 通过调用 http 服务即可调用模型。

相关源码地址：
https://github.com/fengzhizi715/MonicaImageProcessHttpServer
