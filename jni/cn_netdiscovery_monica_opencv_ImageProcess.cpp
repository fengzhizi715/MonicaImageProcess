//
// Created by Tony Shen on 2024/7/14.
//
#include <iostream>
#include "cn_netdiscovery_monica_opencv_ImageProcess.h"
#include "../include/colorcorrection/ColorCorrection.h"
#include "../include/matchTemplate//MatchTemplate.h"
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
    string version = "v0.1.4";
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

JNIEXPORT jlong JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_initColorCorrection
(JNIEnv* env, jobject, jbyteArray array) {
    Mat image = byteArrayToMat(env, array);

    // 创建 C++ 对象并存储指针
    ColorCorrection* cppObject = new ColorCorrection(image);
    return reinterpret_cast<jlong>(cppObject);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_colorCorrection
        (JNIEnv* env, jobject,jbyteArray array, jobject jobj,  jlong cppObjectPtr) {

    ColorCorrection* colorCorrection = reinterpret_cast<ColorCorrection*>(cppObjectPtr);
    ColorCorrectionSettings colorCorrectionSettings;

    Mat image = byteArrayToMat(env, array);

    // 获取 jclass 实例
    jclass jcls = env->FindClass("cn/netdiscovery/monica/ui/controlpanel/colorcorrection/model/ColorCorrectionSettings");
    jfieldID contrastId = env->GetFieldID(jcls, "contrast", "I");
    jfieldID hueId = env->GetFieldID(jcls, "hue", "I");
    jfieldID saturationId = env->GetFieldID(jcls, "saturation", "I");
    jfieldID lightnessId = env->GetFieldID(jcls, "lightness", "I");
    jfieldID temperatureId = env->GetFieldID(jcls, "temperature", "I");
    jfieldID highlightId = env->GetFieldID(jcls, "highlight", "I");
    jfieldID shadowId = env->GetFieldID(jcls, "shadow", "I");
    jfieldID sharpenId = env->GetFieldID(jcls, "sharpen", "I");
    jfieldID cornerId = env->GetFieldID(jcls, "corner", "I");
    jfieldID statusId = env->GetFieldID(jcls, "status", "I");

    colorCorrectionSettings.contrast = env->GetIntField(jobj, contrastId);
    colorCorrectionSettings.hue = env->GetIntField(jobj, hueId);
    colorCorrectionSettings.saturation = env->GetIntField(jobj, saturationId);
    colorCorrectionSettings.lightness = env->GetIntField(jobj, lightnessId);
    colorCorrectionSettings.temperature = env->GetIntField(jobj, temperatureId);
    colorCorrectionSettings.highlight = env->GetIntField(jobj, highlightId);
    colorCorrectionSettings.shadow = env->GetIntField(jobj, shadowId);
    colorCorrectionSettings.sharpen = env->GetIntField(jobj, sharpenId);
    colorCorrectionSettings.corner = env->GetIntField(jobj, cornerId);
    colorCorrectionSettings.status = env->GetIntField(jobj, statusId);

    Mat dst;

    try {
        colorCorrection->doColorCorrection(colorCorrectionSettings, dst);
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

    env->DeleteLocalRef(jcls);  // 手动释放局部引用

    return matToIntArray(env, dst);
}

JNIEXPORT void JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_deleteColorCorrection
        (JNIEnv* env, jobject, jlong cppObjectPtr) {
    // 删除 C++对象，防止内存泄漏
    ColorCorrection* colorCorrection = reinterpret_cast<ColorCorrection*>(cppObjectPtr);
    delete colorCorrection;
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

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_laplaceSharpening
        (JNIEnv* env, jobject, jbyteArray array) {
    Mat image = byteArrayToMat(env,array);
    Mat dst;

    try {
        laplaceSharpening(image,dst);
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

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_threshold
        (JNIEnv* env, jobject,jbyteArray array,jint thresholdType1,jint thresholdType2) {
     Mat image = byteArrayToMat(env,array);

     int type = thresholdType1|thresholdType2;

     Mat thresh;
     int channels = image.channels();
     if (channels == 3) {
         cvtColor(image,image,COLOR_BGR2GRAY);
     }

     threshold(image, thresh,0,255, type);
     return binaryMatToIntArray(env,thresh);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_adaptiveThreshold
        (JNIEnv* env, jobject,jbyteArray array,jint adaptiveMethod, jint thresholdType,jint blockSize, jint c) {
     Mat image = byteArrayToMat(env,array);

     Mat dst;
     int maxVal = 255;
     int channels = image.channels();
     if (channels == 3) {
         cvtColor(image,image,COLOR_BGR2GRAY);
     }

     adaptiveThreshold(image, dst, maxVal, adaptiveMethod,thresholdType, blockSize,c);
     return binaryMatToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_inRange
        (JNIEnv* env, jobject,jbyteArray array,jint hmin,jint smin,jint vmin,jint hmax,jint smax,jint vmax) {
    Mat image = byteArrayToMat(env,array);

    Mat dst;
    inRange(image, dst, hmin, smin, vmin, hmax, smax, vmax);
    return binaryMatToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_roberts
        (JNIEnv* env, jobject,jbyteArray array) {
    Mat image = byteArrayToMat(env,array);

    Mat dst;
    roberts(image, dst);
    return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_prewitt
        (JNIEnv* env, jobject,jbyteArray array) {
    Mat image = byteArrayToMat(env,array);

    Mat dst;
    prewitt(image, dst);
    return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_sobel
        (JNIEnv* env, jobject,jbyteArray array) {
    Mat image = byteArrayToMat(env,array);

    Mat dst;
    sobel(image, dst);
    return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_laplace
        (JNIEnv* env, jobject,jbyteArray array) {
    Mat image = byteArrayToMat(env,array);

    Mat dst;
    laplace(image, dst);
    return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_log
        (JNIEnv* env, jobject,jbyteArray array) {
    Mat image = byteArrayToMat(env,array);

    Mat dst;
    log(image, dst);
    return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_dog
        (JNIEnv* env, jobject,jbyteArray array, jdouble sigma1, jdouble sigma2, jint size) {
    Mat image = byteArrayToMat(env,array);

    Mat dst;
    dog(image, dst, sigma1, sigma2, size);
    return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_canny
        (JNIEnv* env, jobject,jbyteArray array,jdouble threshold1,jdouble threshold2,jint apertureSize) {
    Mat image = byteArrayToMat(env,array);

    Mat dst;
    canny(image, dst, threshold1, threshold2, apertureSize);
    return binaryMatToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_contourAnalysis
        (JNIEnv* env, jobject,jbyteArray srcArray, jbyteArray binaryArray, jobject jobj1, jobject jobj2) {
    ContourFilterSettings contourFilterSettings;
    ContourDisplaySettings contourDisplaySettings;

    Mat src = byteArrayToMat(env, srcArray);
    Mat binary = byteArrayTo8UC1Mat(env,binaryArray);

    // 获取 jclass 实例
    jclass jcls1 = env->FindClass("cn/netdiscovery/monica/ui/controlpanel/ai/experiment/model/ContourFilterSettings");
    jfieldID minPerimeterId = env->GetFieldID(jcls1, "minPerimeter", "D");
    jfieldID maxPerimeterId = env->GetFieldID(jcls1, "maxPerimeter", "D");
    jfieldID minAreaId = env->GetFieldID(jcls1, "minArea", "D");
    jfieldID maxAreaId = env->GetFieldID(jcls1, "maxArea", "D");
    jfieldID minRoundnessId = env->GetFieldID(jcls1, "minRoundness", "D");
    jfieldID maxRoundnessId = env->GetFieldID(jcls1, "maxRoundness", "D");
    jfieldID minAspectRatioId = env->GetFieldID(jcls1, "minAspectRatio", "D");
    jfieldID maxAspectRatioId = env->GetFieldID(jcls1, "maxAspectRatio", "D");

    contourFilterSettings.minPerimeter = env->GetDoubleField(jobj1, minPerimeterId);
    contourFilterSettings.maxPerimeter = env->GetDoubleField(jobj1, maxPerimeterId);
    contourFilterSettings.minArea = env->GetDoubleField(jobj1, minAreaId);
    contourFilterSettings.maxArea = env->GetDoubleField(jobj1, maxAreaId);
    contourFilterSettings.minRoundness = env->GetDoubleField(jobj1, minRoundnessId);
    contourFilterSettings.maxRoundness = env->GetDoubleField(jobj1, maxRoundnessId);
    contourFilterSettings.minAspectRatio = env->GetDoubleField(jobj1, minAspectRatioId);
    contourFilterSettings.maxAspectRatio = env->GetDoubleField(jobj1, maxAspectRatioId);

    // 获取 jclass 实例
    jclass jcls2 = env->FindClass("cn/netdiscovery/monica/ui/controlpanel/ai/experiment/model/ContourDisplaySettings");
    jfieldID showOriginalImageId = env->GetFieldID(jcls2, "showOriginalImage", "Z");
    jfieldID showBoundingRectId = env->GetFieldID(jcls2, "showBoundingRect", "Z");
    jfieldID showMinAreaRectId = env->GetFieldID(jcls2, "showMinAreaRect", "Z");
    jfieldID showCenterId = env->GetFieldID(jcls2, "showCenter", "Z");

    contourDisplaySettings.showOriginalImage = env->GetBooleanField(jobj2, showOriginalImageId);
    contourDisplaySettings.showBoundingRect = env->GetBooleanField(jobj2, showBoundingRectId);
    contourDisplaySettings.showMinAreaRect = env->GetBooleanField(jobj2, showMinAreaRectId);
    contourDisplaySettings.showCenter = env->GetBooleanField(jobj2, showCenterId);

    contourAnalysis(src, binary, contourFilterSettings, contourDisplaySettings);

    env->DeleteLocalRef(jcls1);  // 手动释放局部引用
    env->DeleteLocalRef(jcls2);  // 手动释放局部引用

    if (contourDisplaySettings.showOriginalImage) {
        return matToIntArray(env,src);
    } else {
        return binaryMatToIntArray(env, binary);
    }
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_gaussianBlur
        (JNIEnv* env, jobject,jbyteArray array,jint ksize, jdouble sigmaX, jdouble sigmaY) {
    Mat image = byteArrayToMat(env,array);

    Mat dst;
    GaussianBlur(image, dst, cv::Size(ksize, ksize), sigmaX ,sigmaY);
    return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_medianBlur
        (JNIEnv* env, jobject,jbyteArray array,jint ksize) {
   Mat image = byteArrayToMat(env,array);

   Mat dst;
   medianBlur(image, dst, ksize);
   return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_bilateralFilter
        (JNIEnv* env, jobject,jbyteArray array,jint d,jdouble sigmaColor,jdouble sigmaSpace) {
   Mat image = byteArrayToMat(env,array);

   Mat dst;
   bilateralFilter(image,dst,d, sigmaColor, sigmaSpace);
   return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_pyrMeanShiftFiltering
        (JNIEnv* env, jobject,jbyteArray array,jdouble sp,jdouble sr) {
   Mat image = byteArrayToMat(env,array);

   Mat dst;
   pyrMeanShiftFiltering(image, dst, sp, sr);
   return matToIntArray(env,dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_morphologyEx
        (JNIEnv* env, jobject,jbyteArray array,jobject jobj) {
    MorphologicalOperationSettings morphologicalOperationSettings;

    Mat binary = byteArrayTo8UC1Mat(env, array);
    // 获取 jclass 实例
    jclass jcls = env->FindClass("cn/netdiscovery/monica/ui/controlpanel/ai/experiment/model/MorphologicalOperationSettings");
    jfieldID opId = env->GetFieldID(jcls, "op", "I");
    jfieldID shapeId = env->GetFieldID(jcls, "shape", "I");
    jfieldID widthId = env->GetFieldID(jcls, "width", "I");
    jfieldID heightId = env->GetFieldID(jcls, "height", "I");

    morphologicalOperationSettings.op = env->GetIntField(jobj, opId);
    morphologicalOperationSettings.shape = env->GetIntField(jobj, shapeId);
    morphologicalOperationSettings.width = env->GetIntField(jobj, widthId);
    morphologicalOperationSettings.height = env->GetIntField(jobj, heightId);

    Mat dst;
    morphologyEx(binary, dst, morphologicalOperationSettings);

    env->DeleteLocalRef(jcls);  // 手动释放局部引用

    return binaryMatToIntArray(env, dst);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_matchTemplate
        (JNIEnv* env, jobject,jbyteArray array, jbyteArray arrayTemplate, jobject jobj) {
    MatchTemplateSettings matchTemplateSettings;

    Mat image = byteArrayToMat(env,array);
    Mat templateImage = byteArrayToMat(env,arrayTemplate);
    // 获取 jclass 实例
    jclass jcls = env->FindClass("cn/netdiscovery/monica/ui/controlpanel/ai/experiment/model/MatchTemplateSettings");
    jfieldID matchTypeId = env->GetFieldID(jcls, "matchType", "I");
    jfieldID angleStartId = env->GetFieldID(jcls, "angleStart", "I");
    jfieldID angleEndId = env->GetFieldID(jcls, "angleEnd", "I");
    jfieldID angleStepId = env->GetFieldID(jcls, "angleStep", "I");
    jfieldID scaleStartId = env->GetFieldID(jcls, "scaleStart", "D");
    jfieldID scaleEndId = env->GetFieldID(jcls, "scaleEnd", "D");
    jfieldID scaleStepId = env->GetFieldID(jcls, "scaleStep", "D");
    jfieldID matchTemplateThresholdId = env->GetFieldID(jcls, "matchTemplateThreshold", "D");
    jfieldID scoreThresholdId = env->GetFieldID(jcls, "scoreThreshold", "F");
    jfieldID nmsThresholdId = env->GetFieldID(jcls, "nmsThreshold", "F");

    matchTemplateSettings.matchType = env->GetIntField(jobj, matchTypeId);
    matchTemplateSettings.angleStart = env->GetIntField(jobj, angleStartId);
    matchTemplateSettings.angleEnd = env->GetIntField(jobj, angleEndId);
    matchTemplateSettings.angleStep = env->GetIntField(jobj, angleStepId);
    matchTemplateSettings.scaleStart = env->GetDoubleField(jobj, scaleStartId);
    matchTemplateSettings.scaleEnd = env->GetDoubleField(jobj, scaleEndId);
    matchTemplateSettings.scaleStep = env->GetDoubleField(jobj, scaleStepId);
    matchTemplateSettings.matchTemplateThreshold = env->GetDoubleField(jobj, matchTemplateThresholdId);
    matchTemplateSettings.scoreThreshold = env->GetFloatField(jobj, scoreThresholdId);
    matchTemplateSettings.nmsThreshold = env->GetFloatField(jobj, nmsThresholdId);

    MatchTemplate matchTemplate;

    Mat dst = matchTemplate.templateMatching(image, templateImage, matchTemplateSettings.matchType, matchTemplateSettings.angleStart, matchTemplateSettings.angleEnd, matchTemplateSettings.angleStep,
                                             matchTemplateSettings.scaleStart, matchTemplateSettings.scaleEnd, matchTemplateSettings.scaleStep,
                                             matchTemplateSettings.matchTemplateThreshold, matchTemplateSettings.scoreThreshold, matchTemplateSettings.nmsThreshold);
    return matToIntArray(env,dst);
}




JNIEXPORT void JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_initFaceDetect
        (JNIEnv* env, jobject, jstring jFaceProto, jstring jFaceModel,
         jstring jAgeProto, jstring jAgeModel, jstring jGenderProto, jstring jGenderModel) {

    const char* faceProto = env->GetStringUTFChars(jFaceProto, JNI_FALSE);
    const char* faceModel = env->GetStringUTFChars(jFaceModel, JNI_FALSE);
    const char* ageProto  = env->GetStringUTFChars(jAgeProto, JNI_FALSE);
    const char* ageModel  = env->GetStringUTFChars(jAgeModel, JNI_FALSE);
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