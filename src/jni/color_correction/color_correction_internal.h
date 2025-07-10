//
// Created by Tony on 2025/7/10.
//

#ifndef MONICAIMAGEPROCESS_COLOR_CORRECTION_INTERNAL_H
#define MONICAIMAGEPROCESS_COLOR_CORRECTION_INTERNAL_H

#pragma once
#include <jni.h>

jlong initColorCorrectionInternal(JNIEnv* env, jbyteArray array);

jintArray colorCorrectionInternal(JNIEnv* env, jbyteArray array, jobject jobj, jlong ptr);

void deleteColorCorrectionInternal(JNIEnv* env, jlong ptr);

#endif //MONICAIMAGEPROCESS_COLOR_CORRECTION_INTERNAL_H
