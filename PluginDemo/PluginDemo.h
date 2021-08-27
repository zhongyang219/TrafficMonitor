#pragma once
#include "PluginInterface.h"

#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllexport) IPluginItem* TMPluginCreateInstance(int index);
    __declspec(dllexport) void TMPluginInfoRequired();
    __declspec(dllexport) void TMPluginOptions();

#ifdef __cplusplus
}
#endif
