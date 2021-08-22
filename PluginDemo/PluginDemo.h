#pragma once
#include "PluginInterface.h"

#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllexport) int TMPluginGetItemNum();
    __declspec(dllexport) IPluginItem* TMPluginCreateInstance(int index);
    __declspec(dllexport) void TMPluginInfoRequired();

#ifdef __cplusplus
}
#endif
