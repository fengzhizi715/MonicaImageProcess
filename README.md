# MonicaImageProcess

该项目是为 https://github.com/fengzhizi715/Monica 项目服务的，
使用的 OpenCV C++ 实现的图像处理算法和调用深度学习模型。


项目中各种文件的用途：

| 文件名                                                                                                      | 描述                              |
|----------------------------------------------------------------------------------------------------------|---------------------------------|
| jni/cn_netdiscovery_monica_opencv_ImageProcess.h </br>jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp | jni 对应用层暴露的接口和实现                |
| src/library.cpp                                                                                          | 常规使用 OpenCV 图像算法相关的             |
| src/common/OnnxRuntimeBase.cpp                                                                           | 封装 OnnxRuntime 的基类              |
| src/faceDetect/FaceDetect.cpp                                                                            | 使用 OpenCV 的 dnn 模块加载模型实现的人脸识别检测 |
| src/sketchDrawing/InformativeDrawings.cpp                                                                | 使用 OnnxRuntime 加载模型实现生成素描画      |
| src/faceSwap/Face68Landmarks.cpp                                                                         | 使用 OnnxRuntime 加载模型实现查找人脸的关键点   |
| src/faceSwap/FaceEmbedding.cpp                                                                           | 使用 OnnxRuntime 加载模型实现人脸图像映射     |
| src/faceSwap/FaceEnhance.cpp                                                                             | 使用 OnnxRuntime 加载模型实现人脸增强       |
| src/faceSwap/FaceSwap.cpp                                                                                | 使用 OnnxRuntime 加载模型实现人脸替换       |
| src/faceSwap/Yolov8Face.cpp                                                                              | 使用 OnnxRuntime 加载模型实现人脸检测       |