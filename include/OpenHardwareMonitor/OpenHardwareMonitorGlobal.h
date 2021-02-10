#pragma once

#ifdef OPENHARDWAREMONITOR_EXPORTS
#define OPENHARDWAREMONITOR_API __declspec(dllexport)
#else
#define OPENHARDWAREMONITOR_API __declspec(dllimport)
#endif
