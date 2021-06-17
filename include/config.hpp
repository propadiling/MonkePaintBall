#pragma once

struct config_t {
    int monkemode = 2;
    bool random = true;
    bool enabled = true;
};

extern config_t config;

bool LoadConfig();
void SaveConfig();