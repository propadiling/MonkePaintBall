#pragma once

#include "UnityEngine/Color.hpp"

namespace ColorUtils
{
    UnityEngine::Color GetRandomColor();
    UnityEngine::Color HSVtoRGB(float h, float s, float v);
}