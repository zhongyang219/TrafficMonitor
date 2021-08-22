#pragma once
#include "PluginInterface.h"

#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllexport) int TMPluginGetItemNum();
    __declspec(dllexport) IPluginItem* TMPluginCreateInstance(int index);

#ifdef __cplusplus
}
#endif
