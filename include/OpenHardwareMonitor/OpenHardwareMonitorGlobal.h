#pragma once

#ifdef OPENHARDWAREMONITOR_EXPORTS
#define OPENHARDWAREMONITOR_API __declspec(dllexport)
#else
#define OPENHARDWAREMONITOR_API __declspec(dllimport)
#endif

#define USE_LIBREHARDWAREMONITOR
#ifdef USE_LIBREHARDWAREMONITOR
#define HARDWAREMONITOR_HARDWARE_NAMESPACE                                   LibreHardwareMonitor::Hardware
#define HARDWAREMONITOR_HARDWARE_TYPE_CPU                                    HardwareType::Cpu
#define HARDWAREMONITOR_HARDWARE_TYPE_GPU_NVIDIA                             HardwareType::GpuNvidia
#define HARDWAREMONITOR_HARDWARE_TYPE_GPU_AMD                                HardwareType::GpuAmd
#define HARDWAREMONITOR_HARDWARE_TYPE_STORAGE                                HardwareType::Storage
#define HARDWAREMONITOR_HARDWARE_TYPE_MOTHERBOARD                            HardwareType::Motherboard
#define HARDWAREMONITOR_COMPUTER_GET_HARDWARE_COUNT(computer)                computer->Hardware->Count					
#define HARDWAREMONITOR_COMPUTER_SET_CPU_ENABLED(computer, enable)           computer->IsCpuEnabled = enable
#define HARDWAREMONITOR_COMPUTER_SET_GPU_ENABLED(computer, enable)           computer->IsGpuEnabled = enable
#define HARDWAREMONITOR_COMPUTER_SET_STORAGE_ENABLED(computer, enable)       computer->IsStorageEnabled = enable
#define HARDWAREMONITOR_COMPUTER_SET_MOTHERBOARD_ENABLED(computer, enable)   computer->IsMotherboardEnabled = enable
#else
#define HARDWAREMONITOR_HARDWARE_NAMESPACE                                   OpenHardwareMonitor::Hardware
#define HARDWAREMONITOR_HARDWARE_TYPE_CPU                                    HardwareType::CPU
#define HARDWAREMONITOR_HARDWARE_TYPE_GPU_NVIDIA                             HardwareType::GpuNvidia
#define HARDWAREMONITOR_HARDWARE_TYPE_GPU_AMD                                HardwareType::GpuAti
#define HARDWAREMONITOR_HARDWARE_TYPE_STORAGE                                HardwareType::HDD
#define HARDWAREMONITOR_HARDWARE_TYPE_MOTHERBOARD                            HardwareType::Mainboard
#define HARDWAREMONITOR_COMPUTER_GET_HARDWARE_COUNT(computer)                computer->Hardware->Length					
#define HARDWAREMONITOR_COMPUTER_SET_CPU_ENABLED(computer, enable)           computer->CPUEnabled = enable
#define HARDWAREMONITOR_COMPUTER_SET_GPU_ENABLED(computer, enable)           computer->GPUEnabled = enable
#define HARDWAREMONITOR_COMPUTER_SET_STORAGE_ENABLED(computer, enable)       computer->HDDEnabled = enable
#define HARDWAREMONITOR_COMPUTER_SET_MOTHERBOARD_ENABLED(computer, enable)   computer->MainboardEnabled = enable
#endif // USE_LIBREHARDWAREMONITOR
