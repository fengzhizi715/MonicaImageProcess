//
// Created by Tony on 2025/7/16.
//
#include "format_decoder_internal.h"
#include <libraw/libraw.h>
#include <libheif/heif.h>
#include <iostream>
#include "../../../include/pyramid/PyramidImage.h"
#include "../utils/jni_utils.h"

libraw_processed_image_t* decodeRawInternal(const char *path, jboolean isPreview) {

    LibRaw rawProcessor;

    // 根据 isPreview 设置解码参数
    if (isPreview == JNI_TRUE) {
        rawProcessor.imgdata.params.half_size = 1;             // 快速预览模式（低分辨率）
        rawProcessor.imgdata.params.output_color = 0;          // 禁用色彩空间转换
        rawProcessor.imgdata.params.use_camera_matrix = 0;     // 禁用相机色彩矩阵转换
    } else {
        rawProcessor.imgdata.params.half_size = 0;             // 全尺寸解码
    }

    rawProcessor.imgdata.params.output_bps = 8;                // 输出 8-bit 图像（节省内存）
    rawProcessor.imgdata.params.use_camera_wb = 1;             // 使用相机白平衡
    rawProcessor.imgdata.params.no_auto_bright = 1;            // 禁用自动亮度增强

    if (rawProcessor.open_file(path) != LIBRAW_SUCCESS) {
        std::cerr << "LibRaw failed to open file: " << path << std::endl;
        return nullptr;
    }

    if (rawProcessor.unpack() != LIBRAW_SUCCESS) {
        std::cerr << "LibRaw failed to unpack file: " << path << std::endl;
        rawProcessor.recycle();
        return nullptr;
    }

    if (rawProcessor.dcraw_process() != LIBRAW_SUCCESS) {
        std::cerr << "LibRaw failed to process file: " << path << std::endl;
        rawProcessor.recycle();
        return nullptr;
    }

    libraw_processed_image_t *img = rawProcessor.dcraw_make_mem_image();
    if (!img || img->type != LIBRAW_IMAGE_BITMAP) {
        std::cerr << "LibRaw returned invalid image" << std::endl;
        rawProcessor.recycle();
        return nullptr;
    }

    rawProcessor.recycle();

    return img;
}

jobject decodeRawToBufferInternal(JNIEnv *env, jstring filePath, jboolean isPreview) {

    const char *path = env->GetStringUTFChars(filePath, nullptr);

    libraw_processed_image_t *img = decodeRawInternal(path, isPreview);

    if (img == nullptr) {
        env->ReleaseStringUTFChars(filePath, path);
        return nullptr;
    }

    // 构造 cv::Mat
    int width = img->width;
    int height = img->height;

    cv::Mat mat(height, width, (img->colors == 3) ? CV_8UC3 : CV_8UC1, img->data);
    cv::Mat bgrMat;
    cv::cvtColor(mat, bgrMat, cv::COLOR_RGB2BGR); // RAW 是 RGB 顺序

    // 构造 PyramidImage（内部是异步构建金字塔）
    auto* pyramid = new PyramidImage(bgrMat);

    // 获取预览图像并转 ARGB int array
    cv::Mat preview = pyramid->getPreview();
    jintArray previewArray = matToIntArray(env, preview);

    jclass cls = env->FindClass("cn/netdiscovery/monica/domain/DecodedPreviewImage");
    jmethodID constructor = env->GetMethodID(cls, "<init>", "(JII[I)V");
    jobject result = env->NewObject(cls, constructor, reinterpret_cast<jlong>(pyramid), preview.cols, preview.rows, previewArray);

    LibRaw::dcraw_clear_mem(img);
    env->ReleaseStringUTFChars(filePath, path);

    return result;
}


jobject decodeHeifInternal(JNIEnv *env, jstring filePath) {
    const char *cpath = env->GetStringUTFChars(filePath, nullptr);

    heif_context* ctx = heif_context_alloc();
    heif_error err = heif_context_read_from_file(ctx, cpath, nullptr);
    if (err.code != heif_error_Ok) {
        std::cerr << "Failed to read HEIF: " << err.message << std::endl;
        heif_context_free(ctx);
        env->ReleaseStringUTFChars(filePath, cpath);
        return nullptr;
    }

    heif_image_handle* handle = nullptr;
    err = heif_context_get_primary_image_handle(ctx, &handle);
    if (err.code != heif_error_Ok) {
        std::cerr << "Failed to get primary image handle" << std::endl;
        heif_context_free(ctx);
        env->ReleaseStringUTFChars(filePath, cpath);
        return nullptr;
    }

    heif_image* img = nullptr;
    err = heif_decode_image(handle, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGBA, nullptr);
    if (err.code != heif_error_Ok) {
        std::cerr << "Failed to decode image" << std::endl;
        heif_image_handle_release(handle);
        heif_context_free(ctx);
        env->ReleaseStringUTFChars(filePath, cpath);
        return nullptr;
    }

    int width = heif_image_get_width(img, heif_channel_interleaved);
    int height = heif_image_get_height(img, heif_channel_interleaved);

    int stride = 0;
    const uint8_t* data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);
    if (!data) {
        std::cerr << "Failed to get pixel data" << std::endl;
        heif_image_release(img);
        heif_image_handle_release(handle);
        heif_context_free(ctx);
        env->ReleaseStringUTFChars(filePath, cpath);
        return nullptr;
    }

    // 构建 cv::Mat（RGBA → BGR）
    cv::Mat rgba(height, width, CV_8UC4, (void*)data, stride);
    cv::Mat bgr;
    cv::cvtColor(rgba, bgr, cv::COLOR_RGBA2BGR);

    // 构建 PyramidImage（内部异步金字塔）
    auto* pyramid = new PyramidImage(bgr);

    // 生成预览图并转换为 jintArray
    cv::Mat preview = pyramid->getPreview();
    jintArray previewArray = matToIntArray(env, preview);

    // 构建 DecodedPreviewImage Java 对象
    jclass cls = env->FindClass("cn/netdiscovery/monica/domain/DecodedPreviewImage");
    jmethodID constructor = env->GetMethodID(cls, "<init>", "(JII[I)V");
    jobject result = env->NewObject(cls, constructor, reinterpret_cast<jlong>(pyramid), preview.cols, preview.rows, previewArray);

    // 清理
    heif_image_release(img);
    heif_image_handle_release(handle);
    heif_context_free(ctx);
    env->ReleaseStringUTFChars(filePath, cpath);

    return result;
}