#include "config.hpp"
#include "ColorGun.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Material.hpp"

#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/OVRInput_Button.hpp"
#include "RayCastPlayerSelector.hpp"

#include "Photon/Pun/RpcTarget.hpp"
#include "Photon/Pun/PhotonView.hpp"
#include "Photon/Pun/PhotonNetwork.hpp"

#include "ColorUtils.hpp"

DEFINE_TYPE(PaintBall::Gun);

namespace GorillaUI::BaseGameInterface::PlayerColor {
    UnityEngine::Color get_color();
    Array<Il2CppObject*>* get_colorArray(float r, float g, float b);
}

using namespace UnityEngine;

extern bool allowPaintBall;
static int cooldown = 0;

namespace PaintBall
{
    void Gun::Update()
    {
        using namespace GlobalNamespace;

        if (cooldown > 0) cooldown--;
        rightInput = OVRInput::Get(OVRInput::Button::PrimaryIndexTrigger, OVRInput::Controller::RTouch);
        leftInput = OVRInput::Get(OVRInput::Button::PrimaryIndexTrigger, OVRInput::Controller::LTouch);

        GameObject* obj = nullptr;
        if ((rightInput || leftInput) && allowPaintBall && config.enabled)
        {
            if (rightInput && !RayCastPlayerSelector::useLeftHand)
            {
                if (!wasRightInput)
                {
                    // only do this once
                    wasRightInput = true;

                    obj = RayCastPlayerSelector::get_selectedPlayer();
                }
            }
            else if (rightInput)
            {
                // switch to right
                //RayCastPlayerSelector::useLeftHand = false;
            }
            else if (leftInput && RayCastPlayerSelector::useLeftHand)
            {
                if (!wasLeftInput)
                {
                    // only do this once
                    wasLeftInput = true;

                    obj = RayCastPlayerSelector::get_selectedPlayer();
                }
            }
            else if (leftInput)
            {
                // switch to right
                //RayCastPlayerSelector::useLeftHand = true;
            }
        }
        else
        {
            wasRightInput = false;
            wasLeftInput = false;
        }

        if (obj) // if set to a value, that must mean it was correct
        {
            using namespace Photon::Pun;
            using namespace Photon::Realtime;

            UnityEngine::Color color = config.random ? ColorUtils::GetRandomColor() : GorillaUI::BaseGameInterface::PlayerColor::get_color();
            static Il2CppString* initializeNoobMaterial = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("InitializeNoobMaterial");
            VRRig* rig = obj->GetComponentInParent<GlobalNamespace::VRRig*>();
            
            // cooldown for gorillas so that we dont spam network messages
            if (config.monkemode != 1 && rig && !cooldown) // just gorilla or all
            {
                // about a second of delay
                cooldown = 100;
                PhotonView* photonview = PhotonView::Get(rig);
                photonview->RPC(initializeNoobMaterial, RpcTarget::All, GorillaUI::BaseGameInterface::PlayerColor::get_colorArray(color.r, color.g, color.b));
            }

            if (config.monkemode != 0 && !rig) // just rest or all
            {
                Renderer* renderer = obj->GetComponent<Renderer*>();
                if (!renderer) return;
                Array<Material*>* materials = renderer->get_materials();

                int matCount = materials->Length();
                for (int i = 0; i < matCount; i++)
                {
                    Material* material = materials->values[i];
                    if (material) material->set_color(color);
                }
            }
        }
    }
}