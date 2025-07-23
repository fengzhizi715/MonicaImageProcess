//
// Created by Tony on 2025/7/10.
//
#include "color_correction_internal.h"
#include "../../../include/colorcorrection/ColorCorrection.h"
#include "../utils/jni_utils.h"
#include "../format_decoder/format_decoder_internal.h"
#include "../../../include/PyramidImage.h"

static std::once_flag g_fieldInitFlag;

static struct {
    jclass cls = nullptr;
    jfieldID contrastId;
    jfieldID hueId;
    jfieldID saturationId;
    jfieldID lightnessId;
    jfieldID temperatureId;
    jfieldID highlightId;
    jfieldID shadowId;
    jfieldID sharpenId;
    jfieldID cornerId;
    jfieldID statusId;
} g_colorCorrectionFieldIds;

jlong initColorCorrectionInternal(JNIEnv* env, jbyteArray array) {
    Mat image = byteArrayToMat(env, array);

    // 创建 C++ 对象并存储指针
    ColorCorrection* cppObject = new ColorCorrection(image);
    return reinterpret_cast<jlong>(cppObject);
}


void cacheColorCorrectionFields(JNIEnv* env) {
    std::call_once(g_fieldInitFlag, [&]() {
        jclass localCls = env->FindClass("cn/netdiscovery/monica/domain/ColorCorrectionSettings");
        if (localCls == nullptr) {
            env->FatalError("Failed to find ColorCorrectionSettings class");
            return;
        }

        g_colorCorrectionFieldIds.cls = reinterpret_cast<jclass>(env->NewGlobalRef(localCls));
        env->DeleteLocalRef(localCls); // 清除局部引用

        g_colorCorrectionFieldIds.contrastId    = env->GetFieldID(g_colorCorrectionFieldIds.cls, "contrast", "I");
        g_colorCorrectionFieldIds.hueId         = env->GetFieldID(g_colorCorrectionFieldIds.cls, "hue", "I");
        g_colorCorrectionFieldIds.saturationId  = env->GetFieldID(g_colorCorrectionFieldIds.cls, "saturation", "I");
        g_colorCorrectionFieldIds.lightnessId   = env->GetFieldID(g_colorCorrectionFieldIds.cls, "lightness", "I");
        g_colorCorrectionFieldIds.temperatureId = env->GetFieldID(g_colorCorrectionFieldIds.cls, "temperature", "I");
        g_colorCorrectionFieldIds.highlightId   = env->GetFieldID(g_colorCorrectionFieldIds.cls, "highlight", "I");
        g_colorCorrectionFieldIds.shadowId      = env->GetFieldID(g_colorCorrectionFieldIds.cls, "shadow", "I");
        g_colorCorrectionFieldIds.sharpenId     = env->GetFieldID(g_colorCorrectionFieldIds.cls, "sharpen", "I");
        g_colorCorrectionFieldIds.cornerId      = env->GetFieldID(g_colorCorrectionFieldIds.cls, "corner", "I");
        g_colorCorrectionFieldIds.statusId      = env->GetFieldID(g_colorCorrectionFieldIds.cls, "status", "I");
    });
}

ColorCorrectionSettings extractColorCorrectionSettings(JNIEnv* env, jobject jobj) {
    ColorCorrectionSettings settings;
    settings.contrast    = env->GetIntField(jobj, g_colorCorrectionFieldIds.contrastId);
    settings.hue         = env->GetIntField(jobj, g_colorCorrectionFieldIds.hueId);
    settings.saturation  = env->GetIntField(jobj, g_colorCorrectionFieldIds.saturationId);
    settings.lightness   = env->GetIntField(jobj, g_colorCorrectionFieldIds.lightnessId);
    settings.temperature = env->GetIntField(jobj, g_colorCorrectionFieldIds.temperatureId);
    settings.highlight   = env->GetIntField(jobj, g_colorCorrectionFieldIds.highlightId);
    settings.shadow      = env->GetIntField(jobj, g_colorCorrectionFieldIds.shadowId);
    settings.sharpen     = env->GetIntField(jobj, g_colorCorrectionFieldIds.sharpenId);
    settings.corner      = env->GetIntField(jobj, g_colorCorrectionFieldIds.cornerId);
    settings.status      = env->GetIntField(jobj, g_colorCorrectionFieldIds.statusId);
    return settings;
}

jintArray colorCorrectionInternal(JNIEnv* env, jbyteArray array, jobject jobj, jlong cppObjectPtr) {
    return safeJniCall<jintArray>(env, [&]() -> jintArray {
        if (cppObjectPtr == 0 || jobj == nullptr) {
            return env->NewIntArray(0);
        }

        cacheColorCorrectionFields(env);  // 线程安全调用

        ColorCorrection* colorCorrection = reinterpret_cast<ColorCorrection*>(cppObjectPtr);
        ColorCorrectionSettings settings = extractColorCorrectionSettings(env, jobj);

        Mat image = byteArrayToMat(env, array);
        colorCorrection->origin = image;

        Mat dst;
        colorCorrection->doColorCorrection(settings, dst);

        return matToIntArray(env, dst);
    }, env->NewIntArray(0));
}

jobject decodeRawAndColorCorrectionInternal(JNIEnv* env, jstring filePath, jlong nativePtr, jobject jobj, jlong cppObjectPtr) {
    return safeJniCall<jobject>(env, [&]() -> jobject {
        if (nativePtr == 0 || cppObjectPtr == 0 || jobj == nullptr) {
            return env->NewIntArray(0);
        }

        const char *path = env->GetStringUTFChars(filePath, nullptr);
        cacheColorCorrectionFields(env);  // 保证只初始化一次字段ID等

        ColorCorrection* colorCorrection = reinterpret_cast<ColorCorrection*>(cppObjectPtr);
        ColorCorrectionSettings settings = extractColorCorrectionSettings(env, jobj);

        libraw_processed_image_t *img = decodeRawInternal(path, false);
        if (!img || (img->colors != 3 && img->colors != 1)) {
            std::cerr << "Invalid or unsupported image format" << std::endl;
            if (img) LibRaw::dcraw_clear_mem(img);
            env->ReleaseStringUTFChars(filePath, path);
            return env->NewIntArray(0);
        }

        // 构造 cv::Mat（浅拷贝 + clone 以保证数据安全）
        int width = img->width;
        int height = img->height;
        cv::Mat mat(height, width, (img->colors == 3) ? CV_8UC3 : CV_8UC1, img->data);
        cv::Mat image = mat.clone();
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        // 调色
        colorCorrection->origin = image.clone();
        cv::Mat dst;
        colorCorrection->doColorCorrection(settings, dst);

        // 更新图像金字塔
        PyramidImage* pyramidImage = reinterpret_cast<PyramidImage*>(nativePtr);
        pyramidImage->updateImage(dst);

        // 生成预览图并转换为 jintArray
        cv::Mat preview = pyramidImage->getPreview();
        jintArray previewArray = matToIntArray(env, preview);

        // 构建 DecodedPreviewImage Java 对象
        jclass cls = env->FindClass("cn/netdiscovery/monica/domain/DecodedPreviewImage");
        jmethodID constructor = env->GetMethodID(cls, "<init>", "(JII[I)V");
        jobject result = env->NewObject(cls, constructor, nativePtr, preview.cols, preview.rows, previewArray);

        // 清理资源
        LibRaw::dcraw_clear_mem(img);
        env->ReleaseStringUTFChars(filePath, path);

        return result;
    }, nullptr);
}


jobject colorCorrectionWithPyramidImageInternal(JNIEnv* env, jlong nativePtr, jobject jobj, jlong cppObjectPtr) {
    return safeJniCall<jobject>(env, [&]() -> jobject {
        if (nativePtr == 0 || cppObjectPtr == 0 || jobj == nullptr) {
            return nullptr;
        }

        cacheColorCorrectionFields(env);  // 保证只初始化一次字段ID等

        ColorCorrection* colorCorrection = reinterpret_cast<ColorCorrection*>(cppObjectPtr);
        ColorCorrectionSettings settings = extractColorCorrectionSettings(env, jobj);

        PyramidImage* pyramidImage = reinterpret_cast<PyramidImage*>(nativePtr);
        Mat image = pyramidImage->getOriginal();

        if (image.empty()) {
            std::cerr << "[colorCorrectionWithPyramidImage] original image is empty" << std::endl;
            return nullptr;
        }

        // 调色
        colorCorrection->origin = image.clone();
        cv::Mat dst;
        colorCorrection->doColorCorrection(settings, dst);

        // 更新图像金字塔
        pyramidImage->updateImage(dst);

        // 生成预览图并转换为 jintArray
        cv::Mat preview = pyramidImage->getPreview();
        jintArray previewArray = matToIntArray(env, preview);

        // 构建 DecodedPreviewImage Java 对象
        jclass cls = env->FindClass("cn/netdiscovery/monica/domain/DecodedPreviewImage");
        jmethodID constructor = env->GetMethodID(cls, "<init>", "(JII[I)V");
        jobject result = env->NewObject(cls, constructor, nativePtr, preview.cols, preview.rows, previewArray);

        return result;
    }, nullptr);
}

void deleteColorCorrectionInternal(JNIEnv* env, jlong cppObjectPtr) {
    // 删除 C++对象，防止内存泄漏
    if (cppObjectPtr != 0) {
        delete reinterpret_cast<ColorCorrection*>(cppObjectPtr);
    }
}