# MonicaImageProcess

# 一. 说明
该项目是为 https://github.com/fengzhizi715/Monica 项目服务的，
使用的 OpenCV C++ 实现的图像处理算法和调用、部署深度学习模型。


项目中主要文件的用途：

| 文件名                                                                                                      | 描述                              |
|----------------------------------------------------------------------------------------------------------|---------------------------------|
| jni/cn_netdiscovery_monica_opencv_ImageProcess.h </br>jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp | jni 对应用层暴露的接口和实现                |
| src/library.cpp                                                                                          | 常规使用 OpenCV 图像算法相关的             |
| src/colorcorrection/ColorCorrection.cpp                                                                  | 图像调色相关算法                        |
| src/matchTemplate/MatchTemplate.cpp                                                                      | 模版匹配相关算法                      |
| src/common/OnnxRuntimeBase.cpp                                                                           | 封装 OnnxRuntime 的基类              |
| src/faceDetect/FaceDetect.cpp                                                                            | 使用 OpenCV 的 dnn 模块加载模型实现的人脸识别检测 |
| src/sketchDrawing/InformativeDrawings.cpp                                                                | 使用 OnnxRuntime 加载模型实现生成素描画      |
| src/faceSwap/Face68Landmarks.cpp                                                                         | 使用 OnnxRuntime 加载模型实现查找人脸的关键点   |
| src/faceSwap/FaceEmbedding.cpp                                                                           | 使用 OnnxRuntime 加载模型实现人脸图像映射     |
| src/faceSwap/FaceEnhance.cpp                                                                             | 使用 OnnxRuntime 加载模型实现人脸增强       |
| src/faceSwap/FaceSwap.cpp                                                                                | 使用 OnnxRuntime 加载模型实现人脸替换       |
| src/faceSwap/Yolov8Face.cpp                                                                              | 使用 OnnxRuntime 加载模型实现人脸检测       |

代码目录结构

```
MonicaImageProcess/
├── README.md                       # 项目简介
├── LICENSE                         # 项目许可证文件
├── CMakeLists.txt                  # CMake构建脚本（主）
├── include/                        # 项目公共头文件目录
│   └── colorcorrection/            # 图像调色相关算法的头文件
│   └── common/                     # 工具类的头文件
│   └── faceDetect/                 # 人脸识别的头文件
│   └── faceSwap/                   # 人脸替换的头文件
│   └── matchTemplate/              # 模版匹配的头文件
│   └── sketchDrawing/              # 生成素描画的头文件
│   └── utils/                      # 工具类的头文件
├── jni/                            # 给 Java/Kotlin 调用的本地算法
│       ├── cn_netdiscovery_monica_opencv_ImageProcess.h   # jni 对应用层暴露接口的头文件
│       └── cn_netdiscovery_monica_opencv_ImageProcess.cpp # jni 对应用层暴露接口的源文件
├── src/                             # 项目主源代码目录
│   └── colorcorrection/             # 图像调色相关算法模块
│       └── ColorCorrection.cpp      # 图像调色相关算法的源文件
│   └── common/                      # 工具类的模块
│   └── faceDetect/                  # 人脸识别的模块
│       └── FaceDetect.cpp           # 使用 OpenCV 的 dnn 模块加载模型实现的人脸识别检测的源文件
│   └── faceSwap/                    # 人脸替换的模块
│       └── Face68Landmarks.cpp      # 使用 OnnxRuntime 加载模型实现查找人脸的关键点的源文件
│       └── FaceEmbedding.cpp        # 使用 OnnxRuntime 加载模型实现人脸图像映射的源文件
│       └── FaceEnhance.cpp          # 使用 OnnxRuntime 加载模型实现人脸增强的源文件
│       └── FaceSwap.cpp             # 使用 OnnxRuntime 加载模型实现人脸替换的源文件
│       └── Yolov8Face.cpp           # 使用 OnnxRuntime 加载模型实现人脸检测的源文件
│   └── matchTemplate/               # 模版匹配的模块
│       └── MatchTemplate.cpp        # 模版匹配相关算法的源文件
│   └── server/                      # http 服务的模块
│       ├── CMakeLists.txt           # http 服务模块的构建脚本
│       ├── GlobalResource.cpp       # http 服务加载模型、调用模型的源文件
│       ├── GlobalResource.h         # http 服务加载模型、调用模型的头文件
│       ├── HttpUtils.cpp            # http 服务模块工具类的源文件
│       ├── HttpUtils.h              # http 服务模块工具类的头文件
│       └── main.cpp                 # http 服务模块主程序入口文件
│   └── sketchDrawing/               # 生成素描画的模块
│       └── InformativeDrawings.cpp  # 使用 OnnxRuntime 加载模型实现生成素描画的源文件
│   └── utils/                       # 工具类的模块
│   └── CMakeLists.txt               # 本地算法的构建脚本
└── .gitignore                       # git 忽略和不追踪的文件
```

# 二. 编译方法

## 2.1 本地算法库的编译
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

## 2.2 http server 的编译

1. 进入 server 目录：
```
cd server
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