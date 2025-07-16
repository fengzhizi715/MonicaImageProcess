//
// Created by Tony on 2025/7/16.
//
#include "format_decoder_internal.h"
#include <libraw/libraw.h>
#include <libheif/heif.h>
#include <iostream>

jobject decodeRawToBufferInternal(JNIEnv *env, jstring filePath) {
    const char *path = env->GetStringUTFChars(filePath, nullptr);

    LibRaw rawProcessor;
    if (rawProcessor.open_file(path) != LIBRAW_SUCCESS || rawProcessor.unpack() != LIBRAW_SUCCESS) {
        env->ReleaseStringUTFChars(filePath, path);
        return nullptr;
    }

    rawProcessor.imgdata.params.output_bps = 8;
    rawProcessor.imgdata.params.use_camera_wb = 1;
    rawProcessor.imgdata.params.no_auto_bright = 1;
    rawProcessor.dcraw_process();

    libraw_processed_image_t *img = rawProcessor.dcraw_make_mem_image();

    if (!img || img->type != LIBRAW_IMAGE_BITMAP) {
        rawProcessor.recycle();
        env->ReleaseStringUTFChars(filePath, path);
        return nullptr;
    }

    // 假设 img->colors == 3（RGB）
    int width = img->width;
    int height = img->height;
    int channels = img->colors;
    int length = width * height * channels;

    jbyteArray byteArray = env->NewByteArray(length);
    env->SetByteArrayRegion(byteArray, 0, length, reinterpret_cast<jbyte *>(img->data));

    jclass bufferCls = env->FindClass("cn/netdiscovery/monica/domain/RawImage");
    jmethodID constructor = env->GetMethodID(bufferCls, "<init>", "([BIII)V");
    jobject result = env->NewObject(bufferCls, constructor, byteArray, width, height, channels);

    LibRaw::dcraw_clear_mem(img);
    rawProcessor.recycle();
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

    // 创建 Java int[] pixels
    jintArray pixelArray = env->NewIntArray(width * height);
    jint* pixels = env->GetIntArrayElements(pixelArray, nullptr);

    for (int y = 0; y < height; ++y) {
        const uint8_t* row = data + y * stride;
        for (int x = 0; x < width; ++x) {
            uint8_t r = row[x * 4 + 0];
            uint8_t g = row[x * 4 + 1];
            uint8_t b = row[x * 4 + 2];
            uint8_t a = row[x * 4 + 3];
            pixels[y * width + x] = ((a << 24) | (r << 16) | (g << 8) | b);
        }
    }

    env->ReleaseIntArrayElements(pixelArray, pixels, 0);

    // 构建 HeifImage Java 对象
    jclass heifImageCls = env->FindClass("cn/netdiscovery/monica/domain/HeifImage");
    jmethodID ctor = env->GetMethodID(heifImageCls, "<init>", "(II[I)V");
    jobject heifImage = env->NewObject(heifImageCls, ctor, width, height, pixelArray);

    // 清理
    heif_image_release(img);
    heif_image_handle_release(handle);
    heif_context_free(ctx);
    env->ReleaseStringUTFChars(filePath, cpath);

    return heifImage;
}