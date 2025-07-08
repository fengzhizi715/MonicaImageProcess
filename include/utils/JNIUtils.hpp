//
// Created by Tony on 2025/7/8.
//

#ifndef MONICAIMAGEPROCESS_JNIUTILS_HPP
#define MONICAIMAGEPROCESS_JNIUTILS_HPP

#pragma once
#include <jni.h>
#include <functional>
#include <stdexcept>

template <typename T>
T safeJniCall(JNIEnv* env, const std::function<T()>& nativeLogic, T fallbackValue) {
    try {
        return nativeLogic();
    } catch (const std::exception& e) {
        jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
        if (exceptionClazz != nullptr) {
            env->ThrowNew(exceptionClazz, e.what());
            env->DeleteLocalRef(exceptionClazz);
        }
    } catch (...) {
        jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
        if (exceptionClazz != nullptr) {
            env->ThrowNew(exceptionClazz, "Native error: Unknown exception");
            env->DeleteLocalRef(exceptionClazz);
        }
    }
    return fallbackValue;
}

#endif //MONICAIMAGEPROCESS_JNIUTILS_HPP
