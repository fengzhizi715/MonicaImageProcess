# MonicaImageProcess

该项目是为 https://github.com/fengzhizi715/Monica 服务的，
使用的 OpenCV C++ 实现的图像处理算法和调用深度学习模型实现的推理。


项目中各种文件的用途：

| 文件名                                                                                                      | 描述                   |
|----------------------------------------------------------------------------------------------------------|----------------------|
| library.h、library.cpp                                                                                    | 常规的使用 OpenCV 图像算法相关的 |
| FaceDetect.h、FaceDetect.h                                                                                | 使用深度学习实现的人脸识别检测   |
| jni/cn_netdiscovery_monica_opencv_ImageProcess.h </br>jni/cn_netdiscovery_monica_opencv_ImageProcess.cpp | jni 对应用层暴露的接口        |