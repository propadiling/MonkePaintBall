#include "ColorUtils.hpp"
#include "UnityEngine/Mathf.hpp"

using namespace UnityEngine;

namespace ColorUtils
{
    Color GetRandomColor()
    {
        int d = rand() % 360;
        float h = (float)d / 360.0f;

        Color result = HSVtoRGB(h, 1.0f, 1.0f);

        int r = rand() % 8;
        int m = rand() % 20;
        float mul = (1.0f + ((float)m / 10.0f));

        switch (r)
        {
            case 0:
                result.r *= mul;
                break;
            case 1:
                result.g *= mul;
                break;
            case 2:
                result.b *= mul;
                break;
            case 3:
                result.r *= mul;
                result.g *= mul;
                break;
            case 4:
                result.r *= mul;
                result.b *= mul;
                break;
            case 5:
                result.g *= mul;
                result.b *= mul;
                break;
            case 6:
                result.r *= mul;
                result.g *= mul;
                result.b *= mul;
            default:
                break;
        }

        return result;
    }

    Color HSVtoRGB(float h, float s, float v)
    {
        int hue = (int)(h * 360.0f) % 360;
        float c = v * s;
        float x = c * (1 - Mathf::Abs(((hue / 60) % 2) - 1));
        float m = v - c;

        float r;
        float g;
        float b;
        switch (hue / 60)
        {
            case 0:
                r = c;
                g = x;
                b = 0.0f;
                break;
            case 1:
                r = x;
                g = c;
                b = 0.0f;
                break;
            case 2:
                r = 0.0f;
                g = c;
                b = x;
                break;
            case 3:
                r = 0.0f;
                g = x;
                b = c;
                break;
            case 4:
                r = x;
                g = 0.0f;
                b = c;
                break;
            case 5:
                r = c;
                g = 0.0f;
                b = x;
                break;
            default:
                r = (float)(rand() % 100) / 100.0f;
                g = (float)(rand() % 100) / 100.0f;
                b = (float)(rand() % 100) / 100.0f;
        }

        return Color(r + m, g + m, b + m, 1.0f);
    }
}