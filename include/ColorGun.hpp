#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

DECLARE_CLASS_CODEGEN(PaintBall, Gun, UnityEngine::MonoBehaviour,
    DECLARE_METHOD(void, Update);
    DECLARE_INSTANCE_FIELD(bool, leftInput);
    DECLARE_INSTANCE_FIELD(bool, wasLeftInput);
    DECLARE_INSTANCE_FIELD(bool, rightInput);
    DECLARE_INSTANCE_FIELD(bool, wasRightInput);

    REGISTER_FUNCTION(
        REGISTER_METHOD(Update);
        REGISTER_FIELD(leftInput);
        REGISTER_FIELD(wasLeftInput);
        REGISTER_FIELD(rightInput);
        REGISTER_FIELD(wasRightInput);
    )
)
