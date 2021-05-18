#pragma once

#ifdef OPENHARDWAREMONITOR_EXPORTS
#define OPENHARDWAREMONITOR_API __declspec(dllexport)
#else
#define OPENHARDWAREMONITOR_API __declspec(dllimport)
#endif

#define USE_LIBHARDWAREMONTOR
#ifdef USE_LIBHARDWAREMONTOR
#define HARDWAREMONTOR_HARDWARE_NAMESPACE                                   LibreHardwareMonitor::Hardware
#define HARDWAREMONTOR_HARDWARE_TYPE_CPU                                    HardwareType::Cpu
#define HARDWAREMONTOR_HARDWARE_TYPE_GPU_NVIDIA                             HardwareType::GpuNvidia
#define HARDWAREMONTOR_HARDWARE_TYPE_GPU_AMD                                HardwareType::GpuAmd
#define HARDWAREMONTOR_HARDWARE_TYPE_STORAGE                                HardwareType::Storage
#define HARDWAREMONTOR_HARDWARE_TYPE_MOTHERBOARD                            HardwareType::Motherboard
#define HARDWAREMONTOR_COMPUTER_GET_HARDWARE_COUNT(computer)                computer->Hardware->Count					
#define HARDWAREMONTOR_COMPUTER_SET_CPU_ENABLED(computer, enable)           computer->IsCpuEnabled = enable
#define HARDWAREMONTOR_COMPUTER_SET_GPU_ENABLED(computer, enable)           computer->IsGpuEnabled = enable
#define HARDWAREMONTOR_COMPUTER_SET_STORAGE_ENABLED(computer, enable)       computer->IsStorageEnabled = enable
#define HARDWAREMONTOR_COMPUTER_SET_MOTHERBOARD_ENABLED(computer, enable)   computer->IsMotherboardEnabled = enable
#else
#define HARDWAREMONTOR_HARDWARE_NAMESPACE                                   OpenHardwareMonitor::Hardware
#define HARDWAREMONTOR_HARDWARE_TYPE_CPU                                    HardwareType::CPU
#define HARDWAREMONTOR_HARDWARE_TYPE_GPU_NVIDIA                             HardwareType::GpuNvidia
#define HARDWAREMONTOR_HARDWARE_TYPE_GPU_AMD                                HardwareType::GpuAti
#define HARDWAREMONTOR_HARDWARE_TYPE_STORAGE                                HardwareType::HDD
#define HARDWAREMONTOR_HARDWARE_TYPE_MOTHERBOARD                            HardwareType::Mainboard
#define HARDWAREMONTOR_COMPUTER_GET_HARDWARE_COUNT(computer)                computer->Hardware->Length					
#define HARDWAREMONTOR_COMPUTER_SET_CPU_ENABLED(computer, enable)           computer->CPUEnabled = enable
#define HARDWAREMONTOR_COMPUTER_SET_GPU_ENABLED(computer, enable)           computer->GPUEnabled = enable
#define HARDWAREMONTOR_COMPUTER_SET_STORAGE_ENABLED(computer, enable)       computer->HDDEnabled = enable
#define HARDWAREMONTOR_COMPUTER_SET_MOTHERBOARD_ENABLED(computer, enable)   computer->MainboardEnabled = enable
#endif // USE_LIBHARDWAREMONTOR
