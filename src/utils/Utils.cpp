//
// Created by Tony on 2024/8/18.
//

#include <string>
#include "../../include/utils/Utils.h"

std::wstring get_win_path(const std::string& modelPath) {
    return std::wstring(modelPath.begin(), modelPath.end());
}