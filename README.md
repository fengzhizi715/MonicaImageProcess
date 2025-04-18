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

# 三. http server

http server 部署了几个模型，方便 Monica 无需从本地加载模型文件，只需通过调用 http 服务即可调用模型。

## 3.1 http server 的运行
运行编译后的程序

```
Tony-MacBook-Pro:build tony$ ./MonicaImageProcessHttpServer --help
Allowed options:
  -h [ --help ]                         Display help message
  -p [ --http-port ] arg (=8080)        HTTP server port
  -t [ --num-threads ] arg (=16)        Number of worker threads
  -m [ --model-dir ] arg (=/Users/Tony/IdeaProjects/Monica/resources/common)
                                        Path to the model directory
  -b [ --max-body-size ] arg (=10485760)
                                        Maximum HTTP body size in bytes
```

服务器启动:
```
./MonicaImageProcessHttpServer --http-port 8080 --num-threads 4 --model-dir /Users/Tony/IdeaProjects/Monica/resources/common
```

## 3.2 接口

### 3.2.0 /health
服务器状态检测

### 3.2.1 /api/sketchDrawing 
提供生成素描画的服务

curl 调用的示例:
```
curl -X POST http://localhost:8080/api/sketchDrawing -H "Content-Type: image/jpeg" --data-binary "@/Users/Tony/xxx.png" --output output.jpg
```

## 3.2.2 /api/faceDetect
提供人脸识别的服务

curl 调用的示例:
```
curl -X POST http://localhost:8080/api/faceDetect -H "Content-Type: image/jpeg" --data-binary "@/Users/Tony/xxx.png" --output output.jpg
```

## 3.2.3 /api/faceLandMark
提供人脸检测的服务

curl 调用的示例:
```
curl -X POST http://localhost:8080/api/faceLandMark -H "Content-Type: image/jpeg" --data-binary "@/Users/Tony/xxx.png" --output output.jpg
```

## 3.2.4 /api/faceSwap
提供人脸替换的服务

curl 调用的示例:
```
curl -X POST "http://localhost:8080/api/faceSwap" -H "Content-Type: multipart/form-data" -F "src=@/Users/Tony/src.jpg" -F "target=@/Users/Tony/target.jpg" --output output.jpg
```


## 3.2.5 /api/cartoon
将图像转换成不同的漫画卡通风格

curl 调用的示例:
```
curl -X POST "http://localhost:8080/api/cartoon?type=1" -H "Content-Type: image/jpeg" --data-binary "@/Users/Tony/src.jpg" --output output.jpg
```