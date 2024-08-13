# MonicaImageProcess

该项目是为 https://github.com/fengzhizi715/Monica 服务的，
使用的 OpenCV C++ 实现的图像处理算法和调用深度学习模型实现的推理。


项目中各种文件的用途：

| 文件名                                                                                                      | 描述                  |
|----------------------------------------------------------------------------------------------------------|---------------------|
| library.h </br>library.cpp                                                                               | 常规使用 OpenCV 图像算法相关的 |
| jni/cn_netdiscovery_monica_opencv_ImageProcess.h </br>jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp | jni 对应用层暴露的接口       |
| faceDetect/FaceDetect.h </br>faceDetect/FaceDetect.h                                                     | 使用dnn加载模型实现的人脸识别检测  |