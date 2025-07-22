//
// Created by Tony on 2025/7/10.
//

#ifndef MONICAIMAGEPROCESS_COLOR_CORRECTION_INTERNAL_H
#define MONICAIMAGEPROCESS_COLOR_CORRECTION_INTERNAL_H

#pragma once
#include <jni.h>

jlong initColorCorrectionInternal(JNIEnv* env, jbyteArray array);

jintArray colorCorrectionInternal(JNIEnv* env, jbyteArray array, jobject jobj, jlong cppObjectPtr);

jintArray decodeRawAndColorCorrectionInternal(JNIEnv* env, jstring filePath, jlong nativePtr, jobject jobj, jlong cppObjectPtr);

jobject colorCorrectionWithPyramidImageInternal(JNIEnv* env, jlong nativePtr, jobject jobj, jlong cppObjectPtr);

void deleteColorCorrectionInternal(JNIEnv* env, jlong cppObjectPtr);

#endif //MONICAIMAGEPROCESS_COLOR_CORRECTION_INTERNAL_H
