#pragma once

#include "custom-types/shared/macros.hpp"
#include "monkecomputer/shared/ViewLib/View.hpp"
#include "monkecomputer/shared/InputHandlers/UISelectionHandler.hpp"
#include "monkecomputer/shared/InputHandlers/UIToggleInputHandler.hpp"

DECLARE_CLASS_CODEGEN(PaintBall, PaintBallSettingsView, GorillaUI::Components::View,
    DECLARE_METHOD(void, Awake);
    DECLARE_METHOD(void, DidActivate, bool firstActivation);
    DECLARE_METHOD(void, Redraw);
    DECLARE_METHOD(void, DrawHeader);
    DECLARE_METHOD(void, DrawSettings);
    DECLARE_METHOD(void, OnKeyPressed, int key);
    DECLARE_METHOD(void, OnEnter, int key);

    public:
        GorillaUI::UISelectionHandler* settingSelector = nullptr;
        GorillaUI::UISelectionHandler* monkeModeSelector = nullptr;
        GorillaUI::UISelectionHandler* colorModeSelector = nullptr;
        
    REGISTER_FUNCTION(
        REGISTER_METHOD(Awake);
        REGISTER_METHOD(DidActivate);
        REGISTER_METHOD(Redraw);
        REGISTER_METHOD(DrawHeader);
        REGISTER_METHOD(DrawSettings);
        REGISTER_METHOD(OnKeyPressed);
        REGISTER_METHOD(OnEnter);
    )
)