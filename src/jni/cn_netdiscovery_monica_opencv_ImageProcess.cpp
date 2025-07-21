//
// Created by Tony Shen on 2024/7/14.
//
#include <vector>
#include <cstring>
#include <iostream>
#include "cn_netdiscovery_monica_opencv_ImageProcess.h"
#include "../../include/colorcorrection/ColorCorrection.h"
#include "../../include/matchTemplate/MatchTemplate.h"
#include "../../include/library.h"
#include "../../include/utils/Utils.h"
#include "utils/jni_utils.h"
#include "color_correction/color_correction_internal.h"
#include "format_decoder/format_decoder_internal.h"

MatchTemplate   *match_template = nullptr;

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getVersion
        (JNIEnv* env, jobject) {
    string version = "v0.2.3";
    return env->NewStringUTF(version.c_str());
}

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getOpenCVVersion
        (JNIEnv* env, jobject) {
    string version = CV_VERSION;
    return env->NewStringUTF(version.c_str());
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_shearing
        (JNIEnv* env, jobject,jbyteArray array, jfloat x, jfloat y) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env, array);
        Mat dst = shearing(image, x, y);
        return matToIntArray(env, dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jlong JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_initColorCorrection
(JNIEnv* env, jobject, jbyteArray array) {
    return initColorCorrectionInternal(env, array);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_colorCorrection
        (JNIEnv* env, jobject, jbyteArray array, jobject jobj, jlong cppObjectPtr) {
      return colorCorrectionInternal(env, array, jobj, cppObjectPtr);
}

JNIEXPORT void JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_deleteColorCorrection
        (JNIEnv* env, jobject, jlong cppObjectPtr) {
    deleteColorCorrectionInternal(env, cppObjectPtr);
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_equalizeHist
        (JNIEnv* env, jobject, jbyteArray array) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env, array);
        Mat dst = equalizeHistImage(image);
        return matToIntArray(env, dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_clahe
        (JNIEnv* env, jobject, jbyteArray array, jdouble clipLimit, jint size) {

     return safeJniCall<jintArray>(env, [&]() -> jintArray {
         Mat image = byteArrayToMat(env, array);
         Mat dst;
         clahe(image, dst, clipLimit, size);
         return matToIntArray(env, dst);
     }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_gammaCorrection
        (JNIEnv* env, jobject, jbyteArray array, jfloat k) {

     return safeJniCall<jintArray>(env, [&]() -> jintArray {
         Mat image = byteArrayToMat(env, array);
         Mat dst;
         gammaCorrection(image,dst,k);
         return matToIntArray(env, dst);
     }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_laplaceSharpening
        (JNIEnv* env, jobject, jbyteArray array) {

     return safeJniCall<jintArray>(env, [&]() -> jintArray {
         Mat image = byteArrayToMat(env, array);
         Mat dst;
         laplaceSharpening(image,dst);
         return matToIntArray(env, dst);
     }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_unsharpMask
        (JNIEnv* env, jobject, jbyteArray array, jint radius, jint threshold, jint amount) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env, array);
        Mat dst = Mat(image.size(), image.type());
        unsharpMask(image,dst, radius, threshold, amount);
        return matToIntArray(env, dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_ace
        (JNIEnv* env, jobject, jbyteArray array, jint ratio, jint radius) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env, array);
        Mat dst;
        ace(image,dst,ratio,radius);
        return matToIntArray(env, dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_cvtGray
        (JNIEnv* env, jobject,jbyteArray array) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env, array);
        Mat gray;
        cvtColor(image, gray, COLOR_BGR2GRAY);
        return matToIntArray(env, gray);
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_threshold
        (JNIEnv* env, jobject,jbyteArray array,jint thresholdType1,jint thresholdType2) {

     return safeJniCall<jintArray>(env, [&]() -> jintArray {
         Mat image = byteArrayToMat(env, array);
         int type = thresholdType1|thresholdType2;

         Mat thresh;
        int channels = image.channels();
        if (channels == 3) {
            cvtColor(image,image,COLOR_BGR2GRAY);
        }

        threshold(image, thresh,0,255, type);
        return binaryMatToIntArray(env,thresh);
     }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_adaptiveThreshold
        (JNIEnv* env, jobject,jbyteArray array,jint adaptiveMethod, jint thresholdType,jint blockSize, jint c) {

     return safeJniCall<jintArray>(env, [&]() -> jintArray {
         Mat image = byteArrayToMat(env,array);

         Mat dst;
         int maxVal = 255;
         int channels = image.channels();
         if (channels == 3) {
             cvtColor(image,image,COLOR_BGR2GRAY);
         }

         adaptiveThreshold(image, dst, maxVal, adaptiveMethod,thresholdType, blockSize,c);
         return binaryMatToIntArray(env,dst);
     }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_inRange
        (JNIEnv* env, jobject,jbyteArray array,jint hmin,jint smin,jint vmin,jint hmax,jint smax,jint vmax) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env,array);

        Mat dst;
        inRange(image, dst, hmin, smin, vmin, hmax, smax, vmax);
        return binaryMatToIntArray(env,dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_roberts
        (JNIEnv* env, jobject,jbyteArray array) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env,array);

        Mat dst;
        roberts(image, dst);
        return matToIntArray(env,dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_prewitt
        (JNIEnv* env, jobject,jbyteArray array) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env,array);

        Mat dst;
        prewitt(image, dst);
        return matToIntArray(env,dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_sobel
        (JNIEnv* env, jobject,jbyteArray array) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env,array);

        Mat dst;
        sobel(image, dst);
        return matToIntArray(env,dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_laplace
        (JNIEnv* env, jobject,jbyteArray array) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env,array);

        Mat dst;
        laplace(image, dst);
        return matToIntArray(env,dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_log
        (JNIEnv* env, jobject,jbyteArray array) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env,array);

        Mat dst;
        log(image, dst);
        return matToIntArray(env,dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_dog
        (JNIEnv* env, jobject,jbyteArray array, jdouble sigma1, jdouble sigma2, jint size) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env,array);

        Mat dst;
        dog(image, dst, sigma1, sigma2, size);
        return matToIntArray(env,dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_canny
        (JNIEnv* env, jobject,jbyteArray array,jdouble threshold1,jdouble threshold2,jint apertureSize) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env,array);

        Mat dst;
        canny(image, dst, threshold1, threshold2, apertureSize);
        return binaryMatToIntArray(env,dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_contourAnalysis
        (JNIEnv* env, jobject,jbyteArray srcArray, jbyteArray binaryArray, jintArray scalarArray, jobject jobj1, jobject jobj2) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        ContourFilterSettings contourFilterSettings;
        ContourDisplaySettings contourDisplaySettings;

        Mat src = byteArrayToMat(env, srcArray);
        Mat binary = byteArrayTo8UC1Mat(env,binaryArray);

        jsize len = env->GetArrayLength(scalarArray);
        jint scalarValues[3];
        env->GetIntArrayRegion(scalarArray, 0, len, scalarValues);

        cv::Scalar scalar(scalarValues[0], scalarValues[1], scalarValues[2]);

        // 获取 jclass 实例
        jclass jcls1 = env->FindClass("cn/netdiscovery/monica/domain/ContourFilterSettings");
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
        jclass jcls2 = env->FindClass("cn/netdiscovery/monica/domain/ContourDisplaySettings");
        jfieldID showOriginalImageId = env->GetFieldID(jcls2, "showOriginalImage", "Z");
        jfieldID showBoundingRectId = env->GetFieldID(jcls2, "showBoundingRect", "Z");
        jfieldID showMinAreaRectId = env->GetFieldID(jcls2, "showMinAreaRect", "Z");
        jfieldID showCenterId = env->GetFieldID(jcls2, "showCenter", "Z");

        contourDisplaySettings.showOriginalImage = env->GetBooleanField(jobj2, showOriginalImageId);
        contourDisplaySettings.showBoundingRect = env->GetBooleanField(jobj2, showBoundingRectId);
        contourDisplaySettings.showMinAreaRect = env->GetBooleanField(jobj2, showMinAreaRectId);
        contourDisplaySettings.showCenter = env->GetBooleanField(jobj2, showCenterId);

        contourAnalysis(src, binary, scalar, contourFilterSettings, contourDisplaySettings);

        env->DeleteLocalRef(jcls1);  // 手动释放局部引用
        env->DeleteLocalRef(jcls2);  // 手动释放局部引用

        if (contourDisplaySettings.showOriginalImage) {
            return matToIntArray(env,src);
        } else {
            return binaryMatToIntArray(env, binary);
        }
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_gaussianBlur
        (JNIEnv* env, jobject,jbyteArray array,jint ksize, jdouble sigmaX, jdouble sigmaY) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        Mat image = byteArrayToMat(env,array);

        Mat dst;
        GaussianBlur(image, dst, cv::Size(ksize, ksize), sigmaX ,sigmaY);
        return matToIntArray(env,dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_medianBlur
        (JNIEnv* env, jobject,jbyteArray array,jint ksize) {

   return safeJniCall<jintArray>(env, [&]() -> jintArray {
       Mat image = byteArrayToMat(env,array);

       Mat dst;
       medianBlur(image, dst, ksize);
       return matToIntArray(env,dst);
   }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_bilateralFilter
        (JNIEnv* env, jobject,jbyteArray array,jint d,jdouble sigmaColor,jdouble sigmaSpace) {

   return safeJniCall<jintArray>(env, [&]() -> jintArray {
       Mat image = byteArrayToMat(env,array);

       Mat dst;
       bilateralFilter(image,dst,d, sigmaColor, sigmaSpace);
       return matToIntArray(env,dst);
   }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_pyrMeanShiftFiltering
        (JNIEnv* env, jobject,jbyteArray array,jdouble sp,jdouble sr) {

   return safeJniCall<jintArray>(env, [&]() -> jintArray {
       Mat image = byteArrayToMat(env,array);

       Mat dst;
       pyrMeanShiftFiltering(image, dst, sp, sr);
       return matToIntArray(env,dst);
   }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_morphologyEx
        (JNIEnv* env, jobject,jbyteArray array,jobject jobj) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        MorphologicalOperationSettings morphologicalOperationSettings;

        Mat binary = byteArrayTo8UC1Mat(env, array);
        // 获取 jclass 实例
        jclass jcls = env->FindClass("cn/netdiscovery/monica/domain/MorphologicalOperationSettings");
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
    }, env->NewIntArray(0));
}

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_matchTemplate
        (JNIEnv* env, jobject,jbyteArray array, jbyteArray arrayTemplate, jintArray scalarArray, jobject jobj) {

    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        MatchTemplateSettings matchTemplateSettings;

        Mat image = byteArrayToMat(env,array);
        Mat templateImage = byteArrayToMat(env,arrayTemplate);

        jsize len = env->GetArrayLength(scalarArray);
        jint scalarValues[3];
        env->GetIntArrayRegion(scalarArray, 0, len, scalarValues);

        cv::Scalar scalar(scalarValues[0], scalarValues[1], scalarValues[2]);

        // 获取 jclass 实例
        jclass jcls = env->FindClass("cn/netdiscovery/monica/domain/MatchTemplateSettings");
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

        Mat dst = match_template->templateMatching(image, templateImage, matchTemplateSettings.matchType, matchTemplateSettings.angleStart, matchTemplateSettings.angleEnd, matchTemplateSettings.angleStep,
                                           matchTemplateSettings.scaleStart, matchTemplateSettings.scaleEnd, matchTemplateSettings.scaleStep,
                                           matchTemplateSettings.matchTemplateThreshold, matchTemplateSettings.scoreThreshold, matchTemplateSettings.nmsThreshold, scalar);

        env->DeleteLocalRef(jcls);  // 手动释放局部引用

        return matToIntArray(env,dst);
    }, env->NewIntArray(0));
}

JNIEXPORT jobject JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_decodeRawToBuffer
        (JNIEnv *env, jobject obj, jstring filePath, jboolean isPreview) {

     return safeJniCall<jobject>(env, [&]() -> jobject {
         return decodeRawToBufferInternal(env, filePath, isPreview);
     }, nullptr);
}

JNIEXPORT jobject JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_decodeHeif
        (JNIEnv *env, jobject obj, jstring filePath) {
    return decodeHeifInternal(env, filePath);
}