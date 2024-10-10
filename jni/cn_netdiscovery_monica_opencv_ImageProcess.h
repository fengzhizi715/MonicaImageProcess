//
// Created by Tony Shen on 2024/7/14.
//
#include <jni.h>

#ifndef MONICAIMAGEPROCESS_CN_NETDISCOVERY_MONICA_OPENCV_IMAGEPROCESS_H
#define MONICAIMAGEPROCESS_CN_NETDISCOVERY_MONICA_OPENCV_IMAGEPROCESS_H
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getVersion
        (JNIEnv* env, jobject);

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getOpenCVVersion
        (JNIEnv* env, jobject);

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getONNXRuntimeVersion
        (JNIEnv* env, jobject);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_shearing
        (JNIEnv* env, jobject,jbyteArray array, jfloat x, jfloat y);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_equalizeHist
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_clahe
        (JNIEnv* env, jobject,jbyteArray array,jdouble clipLimit,jint size);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_gammaCorrection
        (JNIEnv* env, jobject,jbyteArray array, jfloat k);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_laplace
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_unsharpMask
        (JNIEnv* env, jobject,jbyteArray array, jint radius, jint threshold, jint amount);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_ace
        (JNIEnv* env, jobject,jbyteArray array, jint ratio, jint radius);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_cvtGray
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_threshold
        (JNIEnv* env, jobject,jbyteArray array,jint thresholdType1,jint thresholdType2);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_adaptiveThreshold
        (JNIEnv* env, jobject,jbyteArray array,jint adaptiveMethod, jint thresholdType, jint blockSize, jint c);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_roberts
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_prewitt
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_sobel
        (JNIEnv* env, jobject,jbyteArray array);



//-------------------------------------------------------------------
// 调用深度学习模型相关
//-------------------------------------------------------------------
JNIEXPORT void JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_initFaceDetect
        (JNIEnv* env, jobject,jstring jFaceProto, jstring jFaceModel,
         jstring jAgeProto, jstring jAgeModel, jstring jGenderProto, jstring jGenderModel);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_faceDetect
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT void JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_initSketchDrawing
        (JNIEnv* env, jobject,jstring jModelPath);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_sketchDrawing
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT void JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_initFaceSwap
        (JNIEnv* env, jobject,jstring jYolov8FaceModelPath, jstring jFace68LandmarksModePath,
         jstring jFaceEmbeddingModePath, jstring jFaceSwapModePath, jstring jFaceSwapModePath2, jstring jFaceEnhanceModePath);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_faceLandMark
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_faceSwap
        (JNIEnv* env, jobject,jbyteArray arraySrc, jbyteArray arrayTarget, jboolean status);

#ifdef __cplusplus
}
#endif
#endif
#pragma once