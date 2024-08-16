# MonicaImageProcess

该项目是为 https://github.com/fengzhizi715/Monica 项目服务的，
使用的 OpenCV C++ 实现的图像处理算法和调用深度学习模型。


项目中各种文件的用途：

| 文件名                                                                                                      | 描述                              |
|----------------------------------------------------------------------------------------------------------|---------------------------------|
| jni/cn_netdiscovery_monica_opencv_ImageProcess.h </br>jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp | jni 对应用层暴露的接口和实现                |
| library.cpp                                                                                              | 常规使用 OpenCV 图像算法相关的             |
| src/common/OnnxRuntimeBase.cpp                                                                           | 封装 OnnxRuntime 的基类              |
| src/faceDetect/FaceDetect.cpp                                                                            | 使用 OpenCV 的 dnn 模块加载模型实现的人脸识别检测 |
| src/sketchDrawing/InformativeDrawings.cpp                         | 使用 onnxruntime 加载模型实现生成素描画      |