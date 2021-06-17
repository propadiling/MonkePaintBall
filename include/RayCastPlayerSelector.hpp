#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/VRRig.hpp"

DECLARE_CLASS_CODEGEN(PaintBall, RayCastPlayerSelector, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(bool, isRight);
    DECLARE_METHOD(void, Update);
    DECLARE_METHOD(void, Awake);
    DECLARE_METHOD(UnityEngine::GameObject*, AttemptToFindPlayer);
    DECLARE_CTOR(ctor);

    DECLARE_METHOD(static UnityEngine::GameObject*, get_selectedPlayer);
    DECLARE_METHOD(static void, disable_point);
    DECLARE_METHOD(static void, set_PointColor, UnityEngine::Color color);
    DECLARE_STATIC_FIELD(UnityEngine::GameObject*, selectedPlayer);
    DECLARE_STATIC_FIELD(UnityEngine::Transform*, raycastEndPoint);
    DECLARE_STATIC_FIELD(bool, useLeftHand);

    REGISTER_FUNCTION(
        REGISTER_METHOD(ctor);
        REGISTER_METHOD(Update);
        REGISTER_METHOD(Awake);
        REGISTER_METHOD(get_selectedPlayer);
        REGISTER_METHOD(AttemptToFindPlayer);
        REGISTER_METHOD(set_PointColor);
        REGISTER_METHOD(disable_point);
        REGISTER_FIELD(isRight);

        REGISTER_FIELD(selectedPlayer);
        REGISTER_FIELD(raycastEndPoint);
        REGISTER_FIELD(useLeftHand);
    )
)