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

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_shearing
        (JNIEnv* env, jobject,jbyteArray array, jfloat x, jfloat y);

JNIEXPORT jlong JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_initColorCorrection
(JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_colorCorrection
        (JNIEnv* env, jobject,jbyteArray array, jobject jobj,  jlong cppObjectPtr);

JNIEXPORT void JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_deleteColorCorrection
        (JNIEnv* env, jobject, jlong cppObjectPtr);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_equalizeHist
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_clahe
        (JNIEnv* env, jobject,jbyteArray array,jdouble clipLimit,jint size);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_gammaCorrection
        (JNIEnv* env, jobject,jbyteArray array, jfloat k);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_laplaceSharpening
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

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_inRange
        (JNIEnv* env, jobject,jbyteArray array,jint hmin,jint smin,jint vmin,jint hmax,jint smax,jint vmax);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_roberts
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_prewitt
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_sobel
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_laplace
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_log
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_dog
        (JNIEnv* env, jobject,jbyteArray array, jdouble sigma1, jdouble sigma2, jint size);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_canny
        (JNIEnv* env, jobject,jbyteArray array,jdouble threshold1,jdouble threshold2,jint apertureSize);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_contourAnalysis
        (JNIEnv* env, jobject,jbyteArray srcArray, jbyteArray binaryArray, jintArray scalarArray, jobject jobj1, jobject jobj2);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_canny
        (JNIEnv* env, jobject,jbyteArray array,jdouble threshold1,jdouble threshold2,jint apertureSize);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_gaussianBlur
        (JNIEnv* env, jobject,jbyteArray array,jint ksize,jdouble sigmaX,jdouble sigmaY);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_medianBlur
        (JNIEnv* env, jobject,jbyteArray array,jint ksize);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_bilateralFilter
        (JNIEnv* env, jobject,jbyteArray array,jint d,jdouble sigmaColor,jdouble sigmaSpace);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_pyrMeanShiftFiltering
        (JNIEnv* env, jobject,jbyteArray array,jdouble sp,jdouble sr);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_morphologyEx
        (JNIEnv* env, jobject,jbyteArray array,jobject jobj);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_matchTemplate
        (JNIEnv* env, jobject,jbyteArray array, jbyteArray arrayTemplate, jintArray scalarArray, jobject jobj);

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_detectRawFormat
        (JNIEnv *env, jobject obj, jstring filePath);

JNIEXPORT jobject JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_decodeRawToBuffer
       (JNIEnv *env, jobject obj, jstring filePath);

#ifdef __cplusplus
}
#endif
#endif
#pragma once