//
// Created by Tony on 2025/7/16.
//

#ifndef MONICAIMAGEPROCESS_FORMAT_DECODER_INTERNAL_H
#define MONICAIMAGEPROCESS_FORMAT_DECODER_INTERNAL_H

#pragma once
#include <jni.h>
#include <libraw/libraw.h>

libraw_processed_image_t* decodeRawInternal(const char *path, jboolean isPreview);

jobject decodeRawToBufferInternal(JNIEnv *env, jstring filePath, jboolean isPreview);

jobject decodeHeifInternal(JNIEnv *env, jstring filePath);

#endif //MONICAIMAGEPROCESS_FORMAT_DECODER_INTERNAL_H
