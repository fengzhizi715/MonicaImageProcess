//
// Created by Tony Shen on 2024/7/14.
//
#include <iostream>
#include "cn_netdiscovery_monica_opencv_ImageProcess.h"
#include "../library.h"
#include "../FaceDetect.h"

FaceDetect faceDetect;

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
    jintArray resultImage = env->NewIntArray(image.total());
    jint *_data = new jint[image.total()];
    for (int i = 0; i < image.total(); i++) {
        char r = image.data[3 * i + 2];
        char g = image.data[3 * i + 1];
        char b = image.data[3 * i + 0];
        char a = (char)255;
        _data[i] = (((jint) a << 24) & 0xFF000000) + (((jint) r << 16) & 0x00FF0000) +
                   (((jint) g << 8) & 0x0000FF00) + ((jint) b & 0x000000FF);
    }
    env->SetIntArrayRegion(resultImage, 0, image.total(), _data);
    delete[]_data;

    return resultImage;
}

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getVersion
        (JNIEnv* env, jobject) {

    string version = getVersion();
    return env->NewStringUTF(version.c_str());
}

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getOpenCVVersion
        (JNIEnv* env, jobject) {

    string version = getOpenCVVersion();
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

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_gammaCorrection
        (JNIEnv* env, jobject,jbyteArray array, jfloat k) {

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
        (JNIEnv* env, jobject,jbyteArray array) {

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
        (JNIEnv* env, jobject,jbyteArray array, jint radius, jint threshold, jint amount) {

    Mat image = byteArrayToMat(env,array);
    Mat dst = Mat(image.size(), image.type());

    try {
        unsharpMask(image,dst, 181, 0, 90);
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
        (JNIEnv* env, jobject,jbyteArray array, jint ratio, jint radius) {

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
        (JNIEnv* env, jobject,jstring jFaceProto, jstring jFaceModel,
         jstring jAgeProto, jstring jAgeModel, jstring jGenderProto, jstring jGenderModel) {

    const char* faceProto = env->GetStringUTFChars(jFaceProto, JNI_FALSE);
    cout << "faceProto path :" << faceProto << endl;
    const char* faceModel = env->GetStringUTFChars(jFaceModel, JNI_FALSE);
    cout << "faceModel path :" << faceModel << endl;
    const char* ageProto = env->GetStringUTFChars(jAgeProto, JNI_FALSE);
    cout << "ageProto path :" << ageProto << endl;
    const char* ageModel = env->GetStringUTFChars(jAgeModel, JNI_FALSE);
    cout << "ageModel path :" << ageModel << endl;
    const char* genderProto = env->GetStringUTFChars(jGenderProto, JNI_FALSE);
    cout << "genderProto path :" << genderProto << endl;
    const char* genderModel = env->GetStringUTFChars(jGenderModel, JNI_FALSE);
    cout << "genderModel path :" << genderModel << endl;

    faceDetect.init(faceProto,faceModel,ageProto,ageModel,genderProto,genderModel);

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
        faceDetect.inferImage(image,dst);
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