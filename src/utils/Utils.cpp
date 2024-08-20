//
// Created by Tony on 2024/8/18.
//

#include <string>
#include "../../include/utils/Utils.h"

// С++ 14 version
//#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
std::wstring get_win_path(const std::string& modelPath) {
    return std::wstring(modelPath.begin(), modelPath.end());
}