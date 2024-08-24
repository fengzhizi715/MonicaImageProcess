//
// Created by Tony Shen on 2024/7/14.
//
#include <iostream>
#include "cn_netdiscovery_monica_opencv_ImageProcess.h"
#include "../include/library.h"
#include "../include/Constants.h"
#include "../include/faceDetect/FaceDetect.h"
#include "../include/sketchDrawing/SketchDrawing.h"
#include "../include/faceSwap/Yolov8Face.h"
#include "../include/faceSwap/Face68Landmarks.h"

FaceDetect      *faceDetect = nullptr;
SketchDrawing   *sketchDrawing = nullptr;
Yolov8Face      *yolov8Face = nullptr;
Face68Landmarks *face68Landmarks = nullptr;

Mat byteArrayToMat(JNIEnv* env, jbyteArray array) {
    //复制java数组到C++
    jsize len = env->GetArrayLength(array);
    signed char* pData = new  signed char[len];
    env->GetByteArrayRegion(array, 0, len, pData);

    // 解码内存数据，变成cv::Mat数据
    cv::Mat image;
    vector<uchar> datas;
    for (int i = 0; i < len; ++i) {
        datas.push_back(pData[i]);
    }
    image = cv::imdecode(datas, IMREAD_COLOR);

    return image;
}

jintArray matToIntArray(JNIEnv *env, const cv::Mat &image) {
    int size = image.total();
    jintArray resultImage = env->NewIntArray(size);
    jint *_data = new jint[size];
    for (int i = 0; i < size; i++) {
        char r = image.data[3 * i + 2];
        char g = image.data[3 * i + 1];
        char b = image.data[3 * i + 0];
        char a = (char)255;
        _data[i] = (((jint) a << 24) & 0xFF000000) + (((jint) r << 16) & 0x00FF0000) +
                   (((jint) g << 8) & 0x0000FF00) + ((jint) b & 0x000000FF);
    }
    env->SetIntArrayRegion(resultImage, 0, size, _data);
    delete[]_data;

    return resultImage;
}

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getVersion
        (JNIEnv* env, jobject) {

    string version = "v0.0.4";
    return env->NewStringUTF(version.c_str());
}

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getOpenCVVersion
        (JNIEnv* env, jobject) {

    string version = CV_VERSION;
    return env->NewStringUTF(version.c_str());
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_shearing
        (JNIEnv* env, jobject,jbyteArray array, jfloat x, jfloat y) {

    Mat image = byteArrayToMat(env,array);
    Mat dst;

    try {
        dst = shearing(image,x,y);
    } catch(...) {
    }

    jthrowable mException = NULL;
    mException = env->ExceptionOccurred();

    if (mException != NULL) {
        env->ExceptionClear();
        jclass exceptionClazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(exceptionClazz, "jni exception");
        env->DeleteLocalRef(exceptionClazz);

        return env->NewIntArray(0);
    }

    return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_equalizeHist
        (JNIEnv* env, jobject, jbyteArray array) {

    Mat image = byteArrayToMat(env,array);
    Mat dst;

    try {
        dst = equalizeHistImage(image);
    } catch(...) {
    }

    jthrowable mException = NULL;
    mException = env->ExceptionOccurred();

    if (mException != NULL) {
        env->ExceptionClear();
        jclass exceptionClazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(exceptionClazz, "jni exception");
        env->DeleteLocalRef(exceptionClazz);

        return env->NewIntArray(0);
    }

    return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_clahe
        (JNIEnv* env, jobject, jbyteArray array, jdouble clipLimit, jint size) {

    Mat image = byteArrayToMat(env,array);
    Mat dst;
    try {
        clahe(image, dst, clipLimit, size);
    } catch(...) {
    }

    jthrowable mException = NULL;
    mException = env->ExceptionOccurred();

    if (mException != NULL) {
        env->ExceptionClear();
        jclass exceptionClazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(exceptionClazz, "jni exception");
        env->DeleteLocalRef(exceptionClazz);

        return env->NewIntArray(0);
    }

    return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_gammaCorrection
        (JNIEnv* env, jobject, jbyteArray array, jfloat k) {

    Mat image = byteArrayToMat(env,array);
    Mat dst;

    try {
        gammaCorrection(image,dst,k);
    } catch(...) {
    }

    jthrowable mException = NULL;
    mException = env->ExceptionOccurred();

    if (mException != NULL) {
        env->ExceptionClear();
        jclass exceptionClazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(exceptionClazz, "jni exception");
        env->DeleteLocalRef(exceptionClazz);

        return env->NewIntArray(0);
    }

    return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_laplace
        (JNIEnv* env, jobject, jbyteArray array) {

    Mat image = byteArrayToMat(env,array);
    Mat dst;

    try {
        laplace(image,dst);
    } catch(...) {
    }

    jthrowable mException = NULL;
    mException = env->ExceptionOccurred();

    if (mException != NULL) {
        env->ExceptionClear();
        jclass exceptionClazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(exceptionClazz, "jni exception");
        env->DeleteLocalRef(exceptionClazz);

        return env->NewIntArray(0);
    }

    return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_unsharpMask
        (JNIEnv* env, jobject, jbyteArray array, jint radius, jint threshold, jint amount) {

    Mat image = byteArrayToMat(env,array);
    Mat dst = Mat(image.size(), image.type());

    try {
        unsharpMask(image,dst, radius, threshold, amount);
    } catch(...) {
    }

    jthrowable mException = NULL;
    mException = env->ExceptionOccurred();

    if (mException != NULL) {
        env->ExceptionClear();
        jclass exceptionClazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(exceptionClazz, "jni exception");
        env->DeleteLocalRef(exceptionClazz);

        return env->NewIntArray(0);
    }

    return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_ace
        (JNIEnv* env, jobject, jbyteArray array, jint ratio, jint radius) {

    Mat image = byteArrayToMat(env,array);
    Mat dst;

    try {
        ace(image,dst,ratio,radius);
    } catch(...) {
    }

    jthrowable mException = NULL;
    mException = env->ExceptionOccurred();

    if (mException != NULL) {
        env->ExceptionClear();
        jclass exceptionClazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(exceptionClazz, "jni exception");
        env->DeleteLocalRef(exceptionClazz);

        return env->NewIntArray(0);
    }

    return matToIntArray(env,dst);
}

JNIEXPORT void JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_initFaceDetect
        (JNIEnv* env, jobject, jstring jFaceProto, jstring jFaceModel,
         jstring jAgeProto, jstring jAgeModel, jstring jGenderProto, jstring jGenderModel) {

    const char* faceProto = env->GetStringUTFChars(jFaceProto, JNI_FALSE);
    const char* faceModel = env->GetStringUTFChars(jFaceModel, JNI_FALSE);
    const char* ageProto = env->GetStringUTFChars(jAgeProto, JNI_FALSE);
    const char* ageModel = env->GetStringUTFChars(jAgeModel, JNI_FALSE);
    const char* genderProto = env->GetStringUTFChars(jGenderProto, JNI_FALSE);
    const char* genderModel = env->GetStringUTFChars(jGenderModel, JNI_FALSE);

    faceDetect = new FaceDetect(faceProto,faceModel,ageProto,ageModel,genderProto,genderModel);

    env->ReleaseStringUTFChars(jFaceProto, faceProto);
    env->ReleaseStringUTFChars(jFaceModel, faceModel);
    env->ReleaseStringUTFChars(jAgeProto, ageProto);
    env->ReleaseStringUTFChars(jAgeModel, ageModel);
    env->ReleaseStringUTFChars(jGenderProto, genderProto);
    env->ReleaseStringUTFChars(jGenderModel, genderModel);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_faceDetect
        (JNIEnv* env, jobject,jbyteArray array) {

    Mat image = byteArrayToMat(env,array);
    Mat dst;

    try {
        faceDetect->inferImage(image,dst);
    } catch(...) {
    }

    jthrowable mException = NULL;
    mException = env->ExceptionOccurred();

    if (mException != NULL) {
        env->ExceptionClear();
        jclass exceptionClazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(exceptionClazz, "jni exception");
        env->DeleteLocalRef(exceptionClazz);

        return env->NewIntArray(0);
    }

    return matToIntArray(env,dst);
}

JNIEXPORT void JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_initSketchDrawing
        (JNIEnv* env, jobject,jstring jModelPath) {

     const char* modelPath = env->GetStringUTFChars(jModelPath, JNI_FALSE);
     const std::string& onnx_logid = "Sketch Drawing";
     const std::string& onnx_provider = OnnxProviders::CPU;
     sketchDrawing = new SketchDrawing(modelPath, onnx_logid.c_str(), onnx_provider.c_str());

     env->ReleaseStringUTFChars(jModelPath, modelPath);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_sketchDrawing
        (JNIEnv* env, jobject,jbyteArray array) {

    Mat image = byteArrayToMat(env,array);
    Mat dst;

    try {
        sketchDrawing-> inferImage(image, dst);
    } catch(...) {
    }

    jthrowable mException = NULL;
    mException = env->ExceptionOccurred();

    if (mException != NULL) {
       env->ExceptionClear();
       jclass exceptionClazz = env->FindClass("java/lang/Exception");
       env->ThrowNew(exceptionClazz, "jni exception");
       env->DeleteLocalRef(exceptionClazz);

       return env->NewIntArray(0);
    }

    if (dst.channels() == 1) {
        cvtColor(dst,dst,COLOR_GRAY2BGR);
    }

    return matToIntArray(env,dst);
}

JNIEXPORT void JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_initFaceSwap
         (JNIEnv* env, jobject,jstring jYolov8FaceModelPath, jstring jFace68LandmarksModePath) {

    const char* yolov8FaceModelPath = env->GetStringUTFChars(jYolov8FaceModelPath, JNI_FALSE);
    const char* face68LandmarksModePath = env->GetStringUTFChars(jFace68LandmarksModePath, JNI_FALSE);

    const std::string& yolov8FaceLogId = "yolov8Face";
    const std::string& face68LandmarksLogId = "face68Landmarks";

    const std::string& onnx_provider = OnnxProviders::CPU;
    yolov8Face = new Yolov8Face(yolov8FaceModelPath, yolov8FaceLogId.c_str(), onnx_provider.c_str());
    face68Landmarks = new Face68Landmarks(face68LandmarksModePath, face68LandmarksLogId.c_str(), onnx_provider.c_str());

    env->ReleaseStringUTFChars(jYolov8FaceModelPath, yolov8FaceModelPath);
    env->ReleaseStringUTFChars(jFace68LandmarksModePath, face68LandmarksModePath);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_faceLandMark
        (JNIEnv* env, jobject,jbyteArray array) {

    Mat image = byteArrayToMat(env,array);
    Mat dst;

    try {
        vector<Bbox> boxes;
        yolov8Face->detect(image, boxes);
        dst = image.clone();
        for (auto box: boxes) {
            rectangle(dst, cv::Point(box.xmin,box.ymin), cv::Point(box.xmax,box.ymax), Scalar(0, 255, 0), 4, 8, 0);

            vector<Point2f> face_landmark_5of68;
            vector<Point2f> face68landmarks = face68Landmarks->detect(image, box, face_landmark_5of68);
            for (auto point : face_landmark_5of68)
            {
                circle(dst, cv::Point(point.x, point.y), 4, Scalar(0, 0, 255), -1);
            }
         }
    } catch(...) {
    }

    jthrowable mException = NULL;
    mException = env->ExceptionOccurred();

    if (mException != NULL) {
        env->ExceptionClear();
        jclass exceptionClazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(exceptionClazz, "jni exception");
        env->DeleteLocalRef(exceptionClazz);

        return env->NewIntArray(0);
    }

    return matToIntArray(env,dst);
}