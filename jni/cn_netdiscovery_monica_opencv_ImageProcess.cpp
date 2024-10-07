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
#include "../include/faceSwap/FaceEmbedding.h"
#include "../include/faceSwap/FaceSwap.h"
#include "../include/faceSwap/FaceEnhance.h"
#include "../include/utils/Utils.h"
#include <onnxruntime_cxx_api.h>

FaceDetect      *faceDetect = nullptr;
SketchDrawing   *sketchDrawing = nullptr;
Yolov8Face      *yolov8Face = nullptr;
Face68Landmarks *face68Landmarks = nullptr;
FaceEmbedding   *faceEmbedding = nullptr;
FaceSwap        *faceSwap = nullptr;
FaceEnhance     *faceEnhance = nullptr;


JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getVersion
        (JNIEnv* env, jobject) {

    string version = "v0.1.0";
    return env->NewStringUTF(version.c_str());
}

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getOpenCVVersion
        (JNIEnv* env, jobject) {

    string version = CV_VERSION;
    return env->NewStringUTF(version.c_str());
}

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getONNXRuntimeVersion
        (JNIEnv* env, jobject) {

    const char* version = OrtGetApiBase()->GetVersionString();
    return env->NewStringUTF(version);
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

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_cvtGray
        (JNIEnv* env, jobject,jbyteArray array) {
    Mat image = byteArrayToMat(env,array);
    Mat gray;
    cvtColor(image,gray,COLOR_BGR2GRAY);

    return matToIntArray(env,gray);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_binary
        (JNIEnv* env, jobject,jbyteArray array,jint typeSelect,jint thresholdSelect) {

     Mat image = byteArrayToMat(env,array);

     int type = typeSelect|thresholdSelect;

     Mat thresh;
     int channels = image.channels();
     if (channels == 3) {
         Mat gray;
         cvtColor(image,gray,COLOR_BGR2GRAY);
         threshold(gray, thresh,0,255, type);
     } else if (channels == 1) {
         threshold(image, thresh,0,255, type);
     }

     return binaryMatToIntArray(env,thresh);
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

    return matToIntArray(env,dst);
}

JNIEXPORT void JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_initFaceSwap
         (JNIEnv* env, jobject,jstring jYolov8FaceModelPath, jstring jFace68LandmarksModePath,
          jstring jFaceEmbeddingModePath, jstring jFaceSwapModePath, jstring jFaceSwapModePath2, jstring jFaceEnhanceModePath){

    const char* yolov8FaceModelPath = env->GetStringUTFChars(jYolov8FaceModelPath, JNI_FALSE);
    const char* face68LandmarksModePath = env->GetStringUTFChars(jFace68LandmarksModePath, JNI_FALSE);
    const char* faceEmbeddingModePath = env->GetStringUTFChars(jFaceEmbeddingModePath, JNI_FALSE);
    const char* faceSwapModePath = env->GetStringUTFChars(jFaceSwapModePath, JNI_FALSE);
    const char* faceSwapModePath2 = env->GetStringUTFChars(jFaceSwapModePath2, JNI_FALSE);
    const char* faceEnhanceModePath = env->GetStringUTFChars(jFaceEnhanceModePath, JNI_FALSE);

    const std::string& yolov8FaceLogId = "yolov8Face";
    const std::string& face68LandmarksLogId = "face68Landmarks";
    const std::string& faceEmbeddingLogId = "faceEmbedding";
    const std::string& faceSwapLogId = "faceSwap";
    const std::string& faceEnhanceLogId = "faceEnhance";

    const std::string& onnx_provider = OnnxProviders::CPU;
    yolov8Face      = new Yolov8Face(yolov8FaceModelPath, yolov8FaceLogId.c_str(), onnx_provider.c_str());
    face68Landmarks = new Face68Landmarks(face68LandmarksModePath, face68LandmarksLogId.c_str(), onnx_provider.c_str());
    yolov8Face      = new Yolov8Face(yolov8FaceModelPath, yolov8FaceLogId.c_str(), onnx_provider.c_str());
    faceEmbedding   = new FaceEmbedding(faceEmbeddingModePath, faceEmbeddingLogId.c_str(), onnx_provider.c_str());
    faceSwap        = new FaceSwap(faceSwapModePath, faceSwapModePath2, faceSwapLogId.c_str(), onnx_provider.c_str());
    faceEnhance     = new FaceEnhance(faceEnhanceModePath, faceEnhanceLogId.c_str(), onnx_provider.c_str());

    env->ReleaseStringUTFChars(jYolov8FaceModelPath, yolov8FaceModelPath);
    env->ReleaseStringUTFChars(jFace68LandmarksModePath, face68LandmarksModePath);
    env->ReleaseStringUTFChars(jFaceEmbeddingModePath, faceEmbeddingModePath);
    env->ReleaseStringUTFChars(jFaceSwapModePath, faceSwapModePath);
    env->ReleaseStringUTFChars(jFaceSwapModePath2, faceSwapModePath2);
    env->ReleaseStringUTFChars(jFaceEnhanceModePath, faceEnhanceModePath);
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
            face68Landmarks->detect(image, box, face_landmark_5of68);
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

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_faceSwap
        (JNIEnv* env, jobject,jbyteArray arraySrc, jbyteArray arrayTarget, jboolean status) {
    Mat src = byteArrayToMat(env,arraySrc);
    Mat target = byteArrayToMat(env,arrayTarget);

    vector<Bbox> boxes;
    yolov8Face->detect(src, boxes);
    int position = 0; // 一张图片里可能有多个人脸，这里只考虑1个人脸的情况

    Bbox firstBox = boxes[position];

    vector<Point2f> face_landmark_5of68;
    face68Landmarks->detect(src, boxes[position], face_landmark_5of68);
    vector<float> source_face_embedding = faceEmbedding->detect(src, face_landmark_5of68);
    yolov8Face -> detect(target, boxes);
    Mat dst = target.clone();

    if (!boxes.empty()) {
        if (status) {
            for (auto box: boxes) {
                vector<Point2f> target_landmark_5;
                face68Landmarks->detect(dst, box, target_landmark_5);

                Mat swap = faceSwap->process(dst, source_face_embedding, target_landmark_5);
                dst = faceEnhance->process(swap, target_landmark_5);
            }
        } else {
            Bbox  box = boxes[0];
            vector<Point2f> target_landmark_5;
            face68Landmarks->detect(dst, box, target_landmark_5);
            Mat swap = faceSwap->process(dst, source_face_embedding, target_landmark_5);
            dst = faceEnhance->process(swap, target_landmark_5);
        }
    }

    return matToIntArray(env,dst);
}