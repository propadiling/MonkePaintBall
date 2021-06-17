#include "config.hpp"
#include "RayCastPlayerSelector.hpp"

#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/Physics.hpp"
#include "UnityEngine/RaycastHit.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/PrimitiveType.hpp"

#include "GlobalNamespace/VRRig.hpp"

#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/OVRInput_Button.hpp"


DEFINE_TYPE(PaintBall::RayCastPlayerSelector);

using namespace UnityEngine;

extern Logger& getLogger();

extern bool allowPaintBall;

namespace PaintBall
{
    bool RayCastPlayerSelector::useLeftHand = false;
    GameObject* RayCastPlayerSelector::selectedPlayer = nullptr;
    Transform* RayCastPlayerSelector::raycastEndPoint = nullptr;

    void RayCastPlayerSelector::ctor()
    {
        isRight = false;
    }

    void RayCastPlayerSelector::Awake()
    {
        if (!raycastEndPoint)
        {
            GameObject* point = GameObject::CreatePrimitive(PrimitiveType::Sphere);
            Object::DontDestroyOnLoad(point);
            raycastEndPoint = point->get_transform();

            Collider* col = point->GetComponent<Collider*>();
            Object::DestroyImmediate(col);
            raycastEndPoint->set_localScale(Vector3::get_one() * 0.1f);
        }
    }

    void RayCastPlayerSelector::Update()
    {
        // if not different, return
        // this means that if they differ, we continue
        if (!(useLeftHand ^ isRight)) return;
        selectedPlayer = AttemptToFindPlayer();
    }

    GameObject* RayCastPlayerSelector::AttemptToFindPlayer()
    {
        RaycastHit hit;
        Transform* transform = get_transform();
        int layermask = 0b1 << 14;
        layermask |= (0b1 << 9);
        if (config.monkemode != 0)
        {
            layermask |= 0b1;
        }

        // hide when not pressing grips
        bool rightInput = GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::PrimaryHandTrigger , GlobalNamespace::OVRInput::Controller::RTouch);
        bool leftInput = GlobalNamespace::OVRInput::Get(GlobalNamespace::OVRInput::Button::PrimaryHandTrigger , GlobalNamespace::OVRInput::Controller::LTouch);
        bool input = rightInput || leftInput;

        // if different the value assigned would be leftInput anyways so this makes it quicker
        if (rightInput ^ leftInput)
        {
            useLeftHand = leftInput;
        }

        if (Physics::Raycast(transform->get_position() + transform->get_forward().get_normalized() * 0.1f, transform->get_forward(), hit, 100.0f, layermask) && input)
        {
            raycastEndPoint->set_position(hit.get_point());
            GameObject* obj = hit.get_collider()->get_gameObject();
            GlobalNamespace::VRRig* rig = obj->GetComponentInParent<GlobalNamespace::VRRig*>();

            switch (config.monkemode)
            {
                case 0: // monke
                    set_PointColor(rig ? Color::get_green() : Color::get_red());
                    return rig ? obj : nullptr;
                case 1: // rest
                    set_PointColor((obj && !rig) ? Color::get_green() : Color::get_red());
                    return !rig ? obj : nullptr;
                case 2: // all
                    set_PointColor(obj ? Color::get_green() : Color::get_red());
                    return obj;
                default:
                    set_PointColor(Color::get_blue());
                    return obj;
            }
        }
        else
        {
            disable_point();
            return nullptr;
        }
    }
    
    GameObject* RayCastPlayerSelector::get_selectedPlayer()
    {
        return selectedPlayer;
    }

    void RayCastPlayerSelector::disable_point()
    {
        if (!raycastEndPoint) return;
        Renderer* renderer = raycastEndPoint->GetComponentInChildren<Renderer*>();
        renderer->set_enabled(false);
    }

    void RayCastPlayerSelector::set_PointColor(Color color)
    {
        if (!raycastEndPoint || !allowPaintBall || !config.enabled) return;

        Renderer* renderer = raycastEndPoint->GetComponentInChildren<Renderer*>();
        renderer->set_enabled(true);
        renderer->get_material()->set_color(color);
    }
}