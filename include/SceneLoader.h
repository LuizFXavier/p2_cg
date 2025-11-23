#pragma once
#include <string>
#include "SceneManager.h"

class SceneLoader {
public:
    static void load(const std::string& filename, SceneManager& manager);
};