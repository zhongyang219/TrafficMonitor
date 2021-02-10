/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	Copyright (C) 2011 Christian Vallières
 
*/

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace OpenHardwareMonitor.Hardware.Nvidia {

  internal enum NvStatus {
    OK = 0,
    ERROR = -1,
    LIBRARY_NOT_FOUND = -2,
    NO_IMPLEMENTATION = -3,
    API_NOT_INTIALIZED = -4,
    INVALID_ARGUMENT = -5,
    NVIDIA_DEVICE_NOT_FOUND = -6,
    END_ENUMERATION = -7,
    INVALID_HANDLE = -8,
    INCOMPATIBLE_STRUCT_VERSION = -9,
    HANDLE_INVALIDATED = -10,
    OPENGL_CONTEXT_NOT_CURRENT = -11,
    NO_GL_EXPERT = -12,
    INSTRUMENTATION_DISABLED = -13,
    EXPECTED_LOGICAL_GPU_HANDLE = -100,
    EXPECTED_PHYSICAL_GPU_HANDLE = -101,
    EXPECTED_DISPLAY_HANDLE = -102,
    INVALID_COMBINATION = -103,
    NOT_SUPPORTED = -104,
    PORTID_NOT_FOUND = -105,
    EXPECTED_UNATTACHED_DISPLAY_HANDLE = -106,
    INVALID_PERF_LEVEL = -107,
    DEVICE_BUSY = -108,
    NV_PERSIST_FILE_NOT_FOUND = -109,
    PERSIST_DATA_NOT_FOUND = -110,
    EXPECTED_TV_DISPLAY = -111,
    EXPECTED_TV_DISPLAY_ON_DCONNECTOR = -112,
    NO_ACTIVE_SLI_TOPOLOGY = -113,
    SLI_RENDERING_MODE_NOTALLOWED = -114,
    EXPECTED_DIGITAL_FLAT_PANEL = -115,
    ARGUMENT_EXCEED_MAX_SIZE = -116,
    DEVICE_SWITCHING_NOT_ALLOWED = -117,
    TESTING_CLOCKS_NOT_SUPPORTED = -118,
    UNKNOWN_UNDERSCAN_CONFIG = -119,
    TIMEOUT_RECONFIGURING_GPU_TOPO = -120,
    DATA_NOT_FOUND = -121,
    EXPECTED_ANALOG_DISPLAY = -122,
    NO_VIDLINK = -123,
    REQUIRES_REBOOT = -124,
    INVALID_HYBRID_MODE = -125,
    MIXED_TARGET_TYPES = -126,
    SYSWOW64_NOT_SUPPORTED = -127,
    IMPLICIT_SET_GPU_TOPOLOGY_CHANGE_NOT_ALLOWED = -128,
    REQUEST_USER_TO_CLOSE_NON_MIGRATABLE_APPS = -129,
    OUT_OF_MEMORY = -130,
    WAS_STILL_DRAWING = -131,
    FILE_NOT_FOUND = -132,
    TOO_MANY_UNIQUE_STATE_OBJECTS = -133,
    INVALID_CALL = -134,
    D3D10_1_LIBRARY_NOT_FOUND = -135,
    FUNCTION_NOT_FOUND = -136
  }

  internal enum NvThermalController {
    NONE = 0,
    GPU_INTERNAL,
    ADM1032,
    MAX6649,
    MAX1617,
    LM99,
    LM89,
    LM64,
    ADT7473,
    SBMAX6649,
    VBIOSEVT,
    OS,
    UNKNOWN = -1,
  }

  internal enum NvThermalTarget {
    NONE = 0,
    GPU = 1,
    MEMORY = 2,
    POWER_SUPPLY = 4,
    BOARD = 8,
    ALL = 15,
    UNKNOWN = -1
  };

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvSensor {
    public NvThermalController Controller;
    public uint DefaultMinTemp;
    public uint DefaultMaxTemp;
    public uint CurrentTemp;
    public NvThermalTarget Target;
  }

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvGPUThermalSettings {
    public uint Version;
    public uint Count;
    [MarshalAs(UnmanagedType.ByValArray,
      SizeConst = NVAPI.MAX_THERMAL_SENSORS_PER_GPU)]
    public NvSensor[] Sensor;
  }

  [StructLayout(LayoutKind.Sequential)]
  internal struct NvDisplayHandle {
    private readonly IntPtr ptr;
  }

  [StructLayout(LayoutKind.Sequential)]
  internal struct NvPhysicalGpuHandle {
    private readonly IntPtr ptr;
  }

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvClocks {
    public uint Version;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = NVAPI.MAX_CLOCKS_PER_GPU)]
    public uint[] Clock;
  }

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvUtilizationDomainEx {
    public bool Present;
    public int Percentage;
  }

  public enum UtilizationDomain {
    GPU,
    FrameBuffer,
    VideoEngine,
    BusInterface
  }

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvDynamicPstatesInfoEx {
    public uint Version;
    public uint Flags;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = NVAPI.NVAPI_MAX_GPU_UTILIZATIONS)]
    public NvUtilizationDomainEx[] UtilizationDomains;
  }


  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvUtilizationDomain {
    public bool Present;
    public int Percentage;
    public ulong Reserved;
  }

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvDynamicPstatesInfo {
    public uint Version;
    public uint Flags;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = NVAPI.NVAPI_MAX_GPU_UTILIZATIONS)]
    public NvUtilizationDomain[] UtilizationDomains;
  }

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvCooler {
    public int Type;
    public int Controller;
    public int DefaultMin;
    public int DefaultMax;
    public int CurrentMin;
    public int CurrentMax;
    public int CurrentLevel;
    public int DefaultPolicy;
    public int CurrentPolicy;
    public int Target;
    public int ControlType;
    public int Active;
  }

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvGPUCoolerSettings {
    public uint Version;
    public uint Count;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = NVAPI.MAX_COOLER_PER_GPU)]
    public NvCooler[] Cooler;
  }

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvLevel {
    public int Level;
    public int Policy;
  }

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvGPUCoolerLevels {
    public uint Version;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = NVAPI.MAX_COOLER_PER_GPU)]
    public NvLevel[] Levels;
  }

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvDisplayDriverMemoryInfo {
    public uint Version;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst =
      NVAPI.MAX_MEMORY_VALUES_PER_GPU)]
    public uint[] Values;
  }

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvDisplayDriverVersion {
    public uint Version;
    public uint DriverVersion;
    public uint BldChangeListNum;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = NVAPI.SHORT_STRING_MAX)]
    public string BuildBranch;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = NVAPI.SHORT_STRING_MAX)]
    public string Adapter;
  }

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvFanCoolersStatus {
    public uint Version;
    public uint Count;

    public ulong Reserved1;
    public ulong Reserved2;
    public ulong Reserved3;
    public ulong Reserved4;

    [MarshalAs(UnmanagedType.ByValArray, 
      SizeConst = NVAPI.MAX_FAN_COOLERS_STATUS_ITEMS)]
    internal NvFanCoolersStatusItem[] Items;
  }

  [StructLayout(LayoutKind.Sequential, Pack = 8)]
  internal struct NvFanCoolersStatusItem {
    public uint Type;
    public uint CurrentRpm;
    public uint CurrentMinLevel;
    public uint CurrentMaxLevel;
    public uint CurrentLevel;

    public uint Reserved1;
    public uint Reserved2;
    public uint Reserved3;
    public uint Reserved4;
    public uint Reserved5;
    public uint Reserved6;
    public uint Reserved7;
    public uint Reserved8;
  }

  internal class NVAPI {

    public const int MAX_PHYSICAL_GPUS = 64;
    public const int SHORT_STRING_MAX = 64;

    public const int MAX_THERMAL_SENSORS_PER_GPU = 3;
    public const int MAX_CLOCKS_PER_GPU = 0x120;
    public const int NVAPI_MAX_GPU_UTILIZATIONS = 8;
    public const int MAX_COOLER_PER_GPU = 20;
    public const int MAX_MEMORY_VALUES_PER_GPU = 5;
    public const int MAX_FAN_COOLERS_STATUS_ITEMS = 32;

    public static readonly uint GPU_THERMAL_SETTINGS_VER = (uint)
      Marshal.SizeOf(typeof(NvGPUThermalSettings)) | 0x10000;
    public static readonly uint GPU_CLOCKS_VER = (uint)
      Marshal.SizeOf(typeof(NvClocks)) | 0x20000;
    public static readonly uint GPU_DYNAMIC_PSTATES_INFO_EX_VER = (uint)
      Marshal.SizeOf(typeof(NvDynamicPstatesInfoEx)) | 0x10000;
    public static readonly uint GPU_DYNAMIC_PSTATES_INFO_VER = (uint)
      Marshal.SizeOf(typeof(NvDynamicPstatesInfo)) | 0x10000;
    public static readonly uint GPU_COOLER_SETTINGS_VER = (uint)
      Marshal.SizeOf(typeof(NvGPUCoolerSettings)) | 0x20000;
    public static readonly uint DISPLAY_DRIVER_MEMORY_INFO_VER = (uint)
      Marshal.SizeOf(typeof(NvDisplayDriverMemoryInfo)) | 0x20000;
    public static readonly uint DISPLAY_DRIVER_VERSION_VER = (uint)
      Marshal.SizeOf(typeof(NvDisplayDriverVersion)) | 0x10000;
    public static readonly uint GPU_COOLER_LEVELS_VER = (uint)
      Marshal.SizeOf(typeof(NvGPUCoolerLevels)) | 0x10000;
    public static readonly uint GPU_FAN_COOLERS_STATUS_VER = (uint)
      Marshal.SizeOf(typeof(NvFanCoolersStatus)) | 0x10000;

    private delegate IntPtr nvapi_QueryInterfaceDelegate(uint id);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate NvStatus NvAPI_InitializeDelegate();

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate NvStatus NvAPI_GPU_GetFullNameDelegate(
      NvPhysicalGpuHandle gpuHandle, StringBuilder name);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GPU_GetThermalSettingsDelegate(
      NvPhysicalGpuHandle gpuHandle, int sensorIndex,
      ref NvGPUThermalSettings gpuThermalSettings);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_EnumNvidiaDisplayHandleDelegate(int thisEnum,
      ref NvDisplayHandle displayHandle);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GetPhysicalGPUsFromDisplayDelegate(
      NvDisplayHandle displayHandle, [Out] NvPhysicalGpuHandle[] gpuHandles,
      out uint gpuCount);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_EnumPhysicalGPUsDelegate(
      [Out] NvPhysicalGpuHandle[] gpuHandles, out int gpuCount);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GPU_GetTachReadingDelegate(
      NvPhysicalGpuHandle gpuHandle, out int value);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GPU_GetAllClocksDelegate(
      NvPhysicalGpuHandle gpuHandle, ref NvClocks clocks);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GPU_GetDynamicPstatesInfoExDelegate(
      NvPhysicalGpuHandle gpuHandle, 
      ref NvDynamicPstatesInfoEx dynamicPstatesInfoEx);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GPU_GetDynamicPstatesInfoDelegate(
      NvPhysicalGpuHandle gpuHandle, 
      ref NvDynamicPstatesInfo dynamicPstatesInfo);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GPU_GetCoolerSettingsDelegate(
      NvPhysicalGpuHandle gpuHandle, int coolerIndex,
      ref NvGPUCoolerSettings gpuCoolerSettings);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GPU_SetCoolerLevelsDelegate(
      NvPhysicalGpuHandle gpuHandle, int coolerIndex,
      ref NvGPUCoolerLevels gpuCoolerLevels);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GetDisplayDriverMemoryInfoDelegate(
      NvDisplayHandle displayHandle, ref NvDisplayDriverMemoryInfo memoryInfo);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GetDisplayDriverVersionDelegate(
      NvDisplayHandle displayHandle, [In, Out] ref NvDisplayDriverVersion
      displayDriverVersion);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GetInterfaceVersionStringDelegate(
      StringBuilder version);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GPU_GetPCIIdentifiersDelegate(
      NvPhysicalGpuHandle gpuHandle, out uint deviceId, out uint subSystemId, 
      out uint revisionId, out uint extDeviceId);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GPU_GetBusIdDelegate(
      NvPhysicalGpuHandle gpuHandle, out uint busId);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate NvStatus NvAPI_GPU_ClientFanCoolersGetStatusDelegate(
      NvPhysicalGpuHandle gpuHandle, ref NvFanCoolersStatus fanCoolersStatus);

    private static readonly bool available;
    private static readonly nvapi_QueryInterfaceDelegate nvapi_QueryInterface;
    private static readonly NvAPI_InitializeDelegate NvAPI_Initialize;
    private static readonly NvAPI_GPU_GetFullNameDelegate
      _NvAPI_GPU_GetFullName;
    private static readonly NvAPI_GetInterfaceVersionStringDelegate
      _NvAPI_GetInterfaceVersionString;

    public static readonly NvAPI_GPU_GetThermalSettingsDelegate
      NvAPI_GPU_GetThermalSettings;
    public static readonly NvAPI_EnumNvidiaDisplayHandleDelegate
      NvAPI_EnumNvidiaDisplayHandle;
    public static readonly NvAPI_GetPhysicalGPUsFromDisplayDelegate
      NvAPI_GetPhysicalGPUsFromDisplay;
    public static readonly NvAPI_EnumPhysicalGPUsDelegate
      NvAPI_EnumPhysicalGPUs;
    public static readonly NvAPI_GPU_GetTachReadingDelegate
      NvAPI_GPU_GetTachReading;
    public static readonly NvAPI_GPU_GetAllClocksDelegate
      NvAPI_GPU_GetAllClocks;
    public static readonly NvAPI_GPU_GetDynamicPstatesInfoExDelegate
      NvAPI_GPU_GetDynamicPstatesInfoEx;
    public static readonly NvAPI_GPU_GetDynamicPstatesInfoDelegate
      NvAPI_GPU_GetDynamicPstatesInfo;
    public static readonly NvAPI_GPU_GetCoolerSettingsDelegate
      NvAPI_GPU_GetCoolerSettings;
    public static readonly NvAPI_GPU_SetCoolerLevelsDelegate
      NvAPI_GPU_SetCoolerLevels;
    public static readonly NvAPI_GetDisplayDriverMemoryInfoDelegate
      NvAPI_GetDisplayDriverMemoryInfo;
    public static readonly NvAPI_GetDisplayDriverVersionDelegate
      NvAPI_GetDisplayDriverVersion;
    public static readonly NvAPI_GPU_GetPCIIdentifiersDelegate
      NvAPI_GPU_GetPCIIdentifiers;
    public static readonly NvAPI_GPU_GetBusIdDelegate
      NvAPI_GPU_GetBusId;
    public static readonly NvAPI_GPU_ClientFanCoolersGetStatusDelegate
      NvAPI_GPU_ClientFanCoolersGetStatus;

    private NVAPI() { }

    public static NvStatus NvAPI_GPU_GetFullName(NvPhysicalGpuHandle gpuHandle,
      out string name) {
      StringBuilder builder = new StringBuilder(SHORT_STRING_MAX);
      NvStatus status;
      if (_NvAPI_GPU_GetFullName != null)
        status = _NvAPI_GPU_GetFullName(gpuHandle, builder);
      else
        status = NvStatus.FUNCTION_NOT_FOUND;
      name = builder.ToString();
      return status;
    }

    public static NvStatus NvAPI_GetInterfaceVersionString(out string version) {
      StringBuilder builder = new StringBuilder(SHORT_STRING_MAX);
      NvStatus status;
      if (_NvAPI_GetInterfaceVersionString != null)
        status = _NvAPI_GetInterfaceVersionString(builder);
      else
        status = NvStatus.FUNCTION_NOT_FOUND;
      version = builder.ToString();
      return status;
    }

    private static string GetDllName() {
      if (IntPtr.Size == 4) {
        return "nvapi.dll";
      } else {
        return "nvapi64.dll";
      }
    }

    private static void GetDelegate<T>(uint id, out T newDelegate)
      where T : class {
      IntPtr ptr = nvapi_QueryInterface(id);
      if (ptr != IntPtr.Zero) {
        newDelegate =
          Marshal.GetDelegateForFunctionPointer(ptr, typeof(T)) as T;
      } else {
        newDelegate = null;
      }
    }

    static NVAPI() {
      DllImportAttribute attribute = new DllImportAttribute(GetDllName());
      attribute.CallingConvention = CallingConvention.Cdecl;
      attribute.PreserveSig = true;
      attribute.EntryPoint = "nvapi_QueryInterface";
      PInvokeDelegateFactory.CreateDelegate(attribute,
        out nvapi_QueryInterface);

      try {
        GetDelegate(0x0150E828, out NvAPI_Initialize);
      } catch (DllNotFoundException) { return; } 
        catch (EntryPointNotFoundException) { return; } 
        catch (ArgumentNullException) { return; }

      if (NvAPI_Initialize() == NvStatus.OK) {
        GetDelegate(0xE3640A56, out NvAPI_GPU_GetThermalSettings);
        GetDelegate(0xCEEE8E9F, out _NvAPI_GPU_GetFullName);
        GetDelegate(0x9ABDD40D, out NvAPI_EnumNvidiaDisplayHandle);
        GetDelegate(0x34EF9506, out NvAPI_GetPhysicalGPUsFromDisplay);
        GetDelegate(0xE5AC921F, out NvAPI_EnumPhysicalGPUs);
        GetDelegate(0x5F608315, out NvAPI_GPU_GetTachReading);
        GetDelegate(0x1BD69F49, out NvAPI_GPU_GetAllClocks);
        GetDelegate(0x60DED2ED, out NvAPI_GPU_GetDynamicPstatesInfoEx);
        GetDelegate(0x189A1FDF, out NvAPI_GPU_GetDynamicPstatesInfo);
        GetDelegate(0xDA141340, out NvAPI_GPU_GetCoolerSettings);
        GetDelegate(0x891FA0AE, out NvAPI_GPU_SetCoolerLevels);
        GetDelegate(0x774AA982, out NvAPI_GetDisplayDriverMemoryInfo);
        GetDelegate(0xF951A4D1, out NvAPI_GetDisplayDriverVersion);
        GetDelegate(0x01053FA5, out _NvAPI_GetInterfaceVersionString);
        GetDelegate(0x2DDFB66E, out NvAPI_GPU_GetPCIIdentifiers);
        GetDelegate(0x1BE0B8E5, out NvAPI_GPU_GetBusId);
        GetDelegate(0x35AED5E8, out NvAPI_GPU_ClientFanCoolersGetStatus);

        available = true;
      }
    }

    public static bool IsAvailable {
      get { return available; }
    }

  }
}
