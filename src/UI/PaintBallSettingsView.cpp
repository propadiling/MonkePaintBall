#include "UI/PaintBallSettingsView.hpp"
#include "monkecomputer/shared/ViewLib/CustomComputer.hpp"
#include "monkecomputer/shared/ViewLib/MonkeWatch.hpp"

#include "config.hpp"

DEFINE_TYPE(PaintBall::PaintBallSettingsView);

extern Logger& getLogger();

using namespace GorillaUI;

namespace PaintBall
{
    void PaintBallSettingsView::Awake()
    {
        settingSelector = new UISelectionHandler(EKeyboardKey::Up, EKeyboardKey::Down, EKeyboardKey::Enter, true, false);
        monkeModeSelector = new UISelectionHandler(EKeyboardKey::Left, EKeyboardKey::Right, EKeyboardKey::Enter, false, true);
        colorModeSelector = new UISelectionHandler(EKeyboardKey::Left, EKeyboardKey::Right, EKeyboardKey::Enter, false, true);

        settingSelector->max = 3;
        monkeModeSelector->max = 3;
        colorModeSelector->max = 2;

        monkeModeSelector->currentSelectionIndex = config.monkemode;
        colorModeSelector->currentSelectionIndex = config.random;
    }

    void PaintBallSettingsView::DidActivate(bool firstActivation)
    {
        std::function<void(int)> fun = std::bind(&PaintBallSettingsView::OnEnter, this, std::placeholders::_1);
        settingSelector->selectionCallback = fun;
        Redraw();
    }

    void PaintBallSettingsView::OnEnter(int index)
    {
        if (index == 0) config.enabled ^= 1;
    }

    void PaintBallSettingsView::Redraw()
    {
        text = "";

        DrawHeader();
        DrawSettings();

        if (computer)
            CustomComputer::Redraw();
        else if (watch)
            MonkeWatch::Redraw();
    }
    
    void PaintBallSettingsView::DrawHeader()
    {
        text += "<color=#ffff00>== <color=#fdfdfd>PaintBall Settings</color> ==</color>\n";
    }
    
    void PaintBallSettingsView::DrawSettings()
    {
        text += "  Paintball is:\n";
        text += settingSelector->currentSelectionIndex == 0 ? " <color=#fd0000>></color> " : "   ";
        text += config.enabled ? "<color=#00fd00>enabled</color>" : "<color=#fd0000>disabled</color>";
        text += "\n";

        text += "  Paint Mode:\n";
        text += settingSelector->currentSelectionIndex == 1 ? " <color=#fd0000>></color> " : "   ";
        text += "<color=#AADDAA><</color> ";
        switch (monkeModeSelector->currentSelectionIndex)
        {
            case 0:
                text += "MONKEY";
                break;
            case 1:
                text += "REST";
                break;
            case 2:
                text += "ALL";
                break;
            default:
                break;
        }
        text += " <color=#AADDAA>></color>";

        text += "\n";
        text += "  Color Source:\n";
        text += settingSelector->currentSelectionIndex == 2 ? " <color=#fd0000>></color> " : "   ";
        text += "<color=#AADDAA><</color> ";
        switch (colorModeSelector->currentSelectionIndex)
        {
            case 0:
                text += "OWN";
                break;
            case 1:
                text += "RANDOM";
                break;
        }
        text += " <color=#AADDAA>></color>";
    }
    
    void PaintBallSettingsView::OnKeyPressed(int value)
    {
        EKeyboardKey key = (EKeyboardKey)value;
        if (!settingSelector->HandleKey(key)) // if it was not up/down/enter
        {
            switch (settingSelector->currentSelectionIndex)
            {
                case 0:
                    break;
                case 1:
                    monkeModeSelector->HandleKey(key);
                    break;
                case 2:
                    colorModeSelector->HandleKey(key);
                    break;
                default:
                    break;
            }

            config.monkemode = monkeModeSelector->currentSelectionIndex;
            config.random = colorModeSelector->currentSelectionIndex;
        }
        Redraw();
    }
}