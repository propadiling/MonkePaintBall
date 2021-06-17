#include "config.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "modloader/shared/modloader.hpp"
#include "monkecomputer/shared/CommandRegister.hpp"
#include "monkecomputer/shared/Register.hpp"
#include "monkecomputer/shared/CustomQueues.hpp"
#include "monkecomputer/shared/GorillaUI.hpp"

#include "GlobalNamespace/GorillaTagger.hpp"
#include "GlobalNamespace/VRRig.hpp"

#include "UnityEngine/Material.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"

#include "Photon/Realtime/Player.hpp"

#include "Photon/Pun/PhotonNetwork.hpp"
#include "Photon/Pun/PhotonView.hpp"
#include "Photon/Pun/RpcTarget.hpp"

#include "GorillaLocomotion/Player.hpp"

#include "custom-types/shared/register.hpp"
#include "RayCastPlayerSelector.hpp"
#include "ColorGun.hpp"
#include "UI/PaintBallSettingsView.hpp"

Logger& getLogger()
{
    static Logger* logger = new Logger({ID, VERSION}, LoggerOptions(false, true));
    return *logger;
}

using namespace UnityEngine;

bool allowPaintBall = true;

MAKE_HOOK_OFFSETLESS(Player_Awake, void, GorillaLocomotion::Player* self)
{
    Player_Awake(self);

    Transform* leftHand = self->get_transform()->Find(il2cpp_utils::createcsstr("TurnParent/LeftHand Controller"));
    Transform* rightHand = self->get_transform()->Find(il2cpp_utils::createcsstr("TurnParent/RightHand Controller"));

    PaintBall::RayCastPlayerSelector* leftSelector = leftHand->get_gameObject()->AddComponent<PaintBall::RayCastPlayerSelector*>();
    leftHand->get_gameObject()->AddComponent<PaintBall::Gun*>();
    PaintBall::RayCastPlayerSelector* rightSelector = rightHand->get_gameObject()->AddComponent<PaintBall::RayCastPlayerSelector*>();
    rightSelector->isRight = true;
    PaintBall::RayCastPlayerSelector::useLeftHand = false;
}

MAKE_HOOK_OFFSETLESS(PhotonNetworkController_OnJoinedRoom, void, Il2CppObject* self)
{
    PhotonNetworkController_OnJoinedRoom(self);

    Il2CppObject* currentRoom = CRASH_UNLESS(il2cpp_utils::RunMethod("Photon.Pun", "PhotonNetwork", "get_CurrentRoom"));

    if (currentRoom)
    {
        // get wether or not this is a private room
        allowPaintBall = !CRASH_UNLESS(il2cpp_utils::RunMethod<bool>(currentRoom, "get_IsVisible"));
        
        // create the c# string which will be the value we want to get from player prefs
        static Il2CppString* currentQueue = il2cpp_utils::createcsstr("currentQueue", il2cpp_utils::StringType::Manual);

        // get the game type (= queue), we registered a game type to the custom pc so if that one is active the mod can be active too
        Il2CppString* queueCS = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>("UnityEngine", "PlayerPrefs", "GetString", currentQueue, il2cpp_utils::createcsstr("DEFAULT")));

        // convert the c# string to a c++ string
        std::string queue = to_utf8(csstrtostr(queueCS));

        // as said before, if the queue is SPACEMONKE, we can allow the mod
        if (queue.find("PAINTBALL") != std::string::npos || queue.find("MODDED") != std::string::npos)
        {
            allowPaintBall = true;
        }
    }
    else allowPaintBall = true;

    // ? construction to switch what is logged, logs work like printf in C with the % placeholders
    getLogger().info("Room Joined! %s", allowPaintBall ? "Room Was Private" : "Room Was not private");

    if (!allowPaintBall) PaintBall::RayCastPlayerSelector::disable_point();
}

MAKE_HOOK_OFFSETLESS(PhotonNetworkController_DisconnectCleanup, void, Il2CppObject* self)
{
    PhotonNetworkController_DisconnectCleanup(self);

    allowPaintBall = true;
}

extern "C" void setup(ModInfo& info)
{
    info.id = ID;
    info.version = VERSION;
}

static int* indexptr = nullptr;
extern "C" void load()
{
    time_t t;
    srand(time(&t));

    if (!LoadConfig()) SaveConfig();
    
    GorillaUI::Innit();
    
    using namespace GlobalNamespace;

    using namespace Photon::Pun;
    using namespace Photon::Realtime;

    Logger& logger = getLogger();
    INSTALL_HOOK_OFFSETLESS(logger, Player_Awake, il2cpp_utils::FindMethodUnsafe("GorillaLocomotion", "Player", "Awake", 0));
    INSTALL_HOOK_OFFSETLESS(logger, PhotonNetworkController_OnJoinedRoom, il2cpp_utils::FindMethodUnsafe("", "PhotonNetworkController", "OnJoinedRoom", 0));
    INSTALL_HOOK_OFFSETLESS(logger, PhotonNetworkController_DisconnectCleanup, il2cpp_utils::FindMethodUnsafe("", "PhotonNetworkController", "DisconnectCleanup", 0));

    custom_types::Register::RegisterType<PaintBall::RayCastPlayerSelector>();
    custom_types::Register::RegisterType<PaintBall::Gun>();
    custom_types::Register::RegisterType<PaintBall::PaintBallSettingsView>();

    GorillaUI::Register::RegisterSettingsView<PaintBall::PaintBallSettingsView*>("Paint Ball", VERSION);
    GorillaUI::Register::RegisterWatchView<PaintBall::PaintBallSettingsView*>("Paint Ball", VERSION);
    GorillaUI::CustomQueues::add_queue("PAINTBALL_CASUAL", "paintball", "  <size=40>A queue for paintballers, shooting eachother with colors!\n    In this queue the paintball mod will remain enabled,\n    Allowing you to duke it out with paint!\n</size>");

    GorillaUI::CommandRegister::RegisterCommand("paint", [](std::vector<std::string> args) -> std::string {
        if (args.size() == 0 ) return "  Invalid Argument(s)!\n  Please use the command as follows:\n  PAINT MODE\n  PAINT MODE [(M)ONKE/(R)EST/(A)LL]\n  PAINT COLOR\n  PAINT COLOR [(R)ANDOM/(O)WN]";

        if (args.size() == 1)
        {
            if (args[0].find("MODE") != std::string::npos)
            {
                switch (config.monkemode)
                {
                    case 0:
                        return "  Paint mode is configured to paint only gorillas";
                        break;
                    case 1:
                        return "  Paint mode is configured to paint everything but gorillas";
                        break;
                    case 2:
                        return "  Paint mode is configured to paint everything";
                        break;
                    default:
                        return "  Invalid Argument(s)!\n  Please use the command as follows:\n  PAINT MODE\n  PAINT MODE [(M)ONKE/(R)EST/(A)LL]\n  PAINT COLOR\n  PAINT COLOR [(R)ANDOM/(O)WN]";
                }
            }
            else if (args[0].find("COLOR") != std::string::npos)
            {
                return config.random ? "  Paint color is configured to be random" : "  Paint color is configured to be copied from your own";
            }
            else if (args[0].find("ENABLE") != std::string::npos) 
            {
                config.enabled = true;
                return "  PaintBall is now enabled";
            }
            else if (args[0].find("DISABLE") != std::string::npos)
            { 
                config.enabled = false;
                return "  PaintBall is now disabled";
            }
            else 
            {
                return "  Invalid Argument(s)!\n  Please use the command as follows:\n  PAINT MODE\n  PAINT MODE [(M)ONKE/(R)EST/(A)LL]\n  PAINT COLOR\n  PAINT COLOR [(R)ANDOM/(O)WN]\n  PAINT [ENABLE/DISABLE]";
            }
        }
        else
        {
            char second = args[1][0];
            if (args[0].find("MODE") != std::string::npos)
            {
                switch (second)
                {
                    case 'M':
                        config.monkemode = 0;
                        return "  Paint mode is now configured to paint only gorillas";
                        break;
                    case 'R':
                        config.monkemode = 1;
                        return "  Paint mode is now configured to paint everything but gorillas";
                        break;
                    case 'A':
                        config.monkemode = 2;
                        return "  Paint mode is now configured to paint everything";
                        break;
                    default:
                        return "  Invalid Argument(s)!\n  Please use the command as follows:\n  PAINT MODE\n  PAINT MODE [(M)ONKE/(R)EST/(A)LL]";
                }
            }
            else if (args[0].find("COLOR") != std::string::npos)
            {
                switch (second)
                {
                    case 'R':
                        config.random = true;
                        break;
                    case 'O':
                        config.random = false;
                        break;
                    default:
                        return "  Invalid Argument(s)!\n  Please use the command as follows:\n  PAINT COLOR\n  PAINT COLOR [(R)ANDOM/(O)WN]";
                }
                return config.random ? "  Paint color is configured to be random" : "  Paint color is configured to be copied from your own";
            }
            else if (args[0].find("ENABLE") != std::string::npos) 
            {
                config.enabled = true;
                return "  PaintBall is now enabled";
            }
            else if (args[0].find("DISABLE") != std::string::npos)
            { 
                config.enabled = false;
                return "  PaintBall is now disabled";
            }
            else 
            {
                return "  Invalid Argument(s)!\n  Please use the command as follows:\n  PAINT MODE\n  PAINT MODE [(M)ONKE/(R)EST/(A)LL]\n  PAINT COLOR\n  PAINT COLOR [(R)ANDOM/(O)WN]";
            }
        }

    }, "Allows you to change the way painting works, using arguments to specify what is being changed\n  Please use the command as follows:\n  PAINT MODE\n  PAINT MODE [(M)ONKE/(R)EST/(A)LL]\n  PAINT COLOR\n  PAINT COLOR [(R)ANDOM/(O)WN]\n  PAINT [ENABLE/DISABLE]");
    
    /*
    GorillaUI::CommandRegister::RegisterCommand("material", [](std::vector<std::string> args) -> std::string {
        if (args.size() == 0) return "  Invalid Argument!";
        static Il2CppString* changeMaterial = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("ChangeMaterial");

        GorillaTagger* gorillaTagger = GorillaTagger::get_Instance();

        VRRig* offlineRig = gorillaTagger->offlineVRRig;

        int count = offlineRig ? offlineRig->materialsToChangeTo->Length() : 0;
        int index = atoi(args[0].c_str());

        if (offlineRig)
        {
            if (index >= count)
            {
                return string_format("  Index out of range!\n  Only %d materials exist", count);
            }
            else
            {
                offlineRig->ChangeMaterial(index);
            }
        }

        if (PhotonNetwork::get_InRoom())
        {
            VRRig* myVRRig = gorillaTagger->myVRRig;

            count = myVRRig ? myVRRig->materialsToChangeTo->Length() : 0;

            if (myVRRig)
            {
                if (index >= count)
                {
                    return string_format("  Index out of range!\n  Only %d materials exist", count);
                }
                else
                {
                    myVRRig->ChangeMaterial(index);
                    if (!indexptr) indexptr = new int;
                    *indexptr = index;

                    Array<Il2CppObject*>* arr = reinterpret_cast<Array<Il2CppObject*>*>(il2cpp_functions::array_new(classof(Il2CppObject*), 1));
                    arr->values[0] = il2cpp_functions::value_box(classof(int), indexptr);

                    Array<Player*>* players = PhotonNetwork::get_PlayerList();
                    int playerCount = players->Length(); 
                    
                    for (int i = 0; i < playerCount; i++)
                    {
                        Player* player = players->values[i];
                        PhotonView::Get(myVRRig)->RPC(changeMaterial, player, arr);
                    }
                }
            }
        }
        return string_format("  Set material to %d out of %d", index, count);
    }, "forces your current material to whatever number you put if it is within range");
    */
}