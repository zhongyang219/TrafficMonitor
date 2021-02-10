/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2012 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Runtime.InteropServices;

namespace OpenHardwareMonitor.Hardware.TBalancer {

  internal enum FT_DEVICE : uint {
    FT_DEVICE_232BM,
    FT_DEVICE_232AM,
    FT_DEVICE_100AX,
    FT_DEVICE_UNKNOWN,
    FT_DEVICE_2232C,
    FT_DEVICE_232R,
    FT_DEVICE_2232H,
    FT_DEVICE_4232H
  }

  internal enum FT_STATUS {
    FT_OK,
    FT_INVALID_HANDLE,
    FT_DEVICE_NOT_FOUND,
    FT_DEVICE_NOT_OPENED,
    FT_IO_ERROR,
    FT_INSUFFICIENT_RESOURCES,
    FT_INVALID_PARAMETER,
    FT_INVALID_BAUD_RATE,
    FT_DEVICE_NOT_OPENED_FOR_ERASE,
    FT_DEVICE_NOT_OPENED_FOR_WRITE,
    FT_FAILED_TO_WRITE_DEVICE,
    FT_EEPROM_READ_FAILED,
    FT_EEPROM_WRITE_FAILED,
    FT_EEPROM_ERASE_FAILED,
    FT_EEPROM_NOT_PRESENT,
    FT_EEPROM_NOT_PROGRAMMED,
    FT_INVALID_ARGS,
    FT_OTHER_ERROR
  }

  internal enum FT_FLOW_CONTROL : ushort {
    FT_FLOW_DTR_DSR = 512,
    FT_FLOW_NONE = 0,
    FT_FLOW_RTS_CTS = 256,
    FT_FLOW_XON_XOFF = 1024,
  }

  internal enum FT_PURGE : uint {
    FT_PURGE_RX = 1,
    FT_PURGE_TX = 2,
    FT_PURGE_ALL = 3,
  }

  [StructLayout(LayoutKind.Sequential)]
  internal struct FT_HANDLE {
    private readonly uint handle;
  }

  [StructLayout(LayoutKind.Sequential)]
  internal struct FT_DEVICE_INFO_NODE {    
    public uint Flags;
    public FT_DEVICE Type; 
    public uint ID; 
    public uint LocId; 
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
    public string SerialNumber; 
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string Description;
    public FT_HANDLE Handle;
  }

  internal class FTD2XX {

    public delegate FT_STATUS FT_CreateDeviceInfoListDelegate(
      out uint numDevices);
    public delegate FT_STATUS FT_GetDeviceInfoListDelegate(
      [Out] FT_DEVICE_INFO_NODE[] deviceInfoNodes, ref uint length);
    public delegate FT_STATUS FT_OpenDelegate(int device, out FT_HANDLE handle);
    public delegate FT_STATUS FT_CloseDelegate(FT_HANDLE handle);
    public delegate FT_STATUS FT_SetBaudRateDelegate(FT_HANDLE handle,
      uint baudRate);
    public delegate FT_STATUS FT_SetDataCharacteristicsDelegate(
      FT_HANDLE handle, byte wordLength, byte stopBits, byte parity);
    public delegate FT_STATUS FT_SetFlowControlDelegate(FT_HANDLE handle,
      FT_FLOW_CONTROL flowControl, byte xon, byte xoff);
    public delegate FT_STATUS FT_SetTimeoutsDelegate(FT_HANDLE handle,
      uint readTimeout, uint writeTimeout);
    public delegate FT_STATUS FT_WriteDelegate(FT_HANDLE handle, byte[] buffer,
      uint bytesToWrite, out uint bytesWritten);
    public delegate FT_STATUS FT_PurgeDelegate(FT_HANDLE handle, FT_PURGE mask);
    public delegate FT_STATUS FT_GetStatusDelegate(FT_HANDLE handle,
      out uint amountInRxQueue, out uint amountInTxQueue, out uint eventStatus);
    public delegate FT_STATUS FT_ReadDelegate(FT_HANDLE handle, 
      [Out] byte[] buffer, uint bytesToRead, out uint bytesReturned);
    public delegate FT_STATUS FT_ReadByteDelegate(FT_HANDLE handle,
      out byte buffer, uint bytesToRead, out uint bytesReturned);

    public static readonly FT_CreateDeviceInfoListDelegate 
      FT_CreateDeviceInfoList = CreateDelegate<
      FT_CreateDeviceInfoListDelegate>("FT_CreateDeviceInfoList");
    public static readonly FT_GetDeviceInfoListDelegate 
      FT_GetDeviceInfoList = CreateDelegate<
      FT_GetDeviceInfoListDelegate>("FT_GetDeviceInfoList");
    public static readonly FT_OpenDelegate 
      FT_Open = CreateDelegate<
      FT_OpenDelegate>("FT_Open");
    public static readonly FT_CloseDelegate 
      FT_Close = CreateDelegate<
      FT_CloseDelegate>("FT_Close");
    public static readonly FT_SetBaudRateDelegate 
      FT_SetBaudRate = CreateDelegate<
      FT_SetBaudRateDelegate>("FT_SetBaudRate");
    public static readonly FT_SetDataCharacteristicsDelegate 
      FT_SetDataCharacteristics = CreateDelegate<
      FT_SetDataCharacteristicsDelegate>("FT_SetDataCharacteristics");
    public static readonly FT_SetFlowControlDelegate 
      FT_SetFlowControl = CreateDelegate<
      FT_SetFlowControlDelegate>("FT_SetFlowControl");
    public static readonly FT_SetTimeoutsDelegate 
      FT_SetTimeouts = CreateDelegate<
      FT_SetTimeoutsDelegate>("FT_SetTimeouts");
    public static readonly FT_WriteDelegate 
      FT_Write = CreateDelegate<
      FT_WriteDelegate>("FT_Write");
    public static readonly FT_PurgeDelegate 
      FT_Purge = CreateDelegate<
      FT_PurgeDelegate>("FT_Purge");
    public static readonly FT_GetStatusDelegate 
      FT_GetStatus = CreateDelegate<
      FT_GetStatusDelegate>("FT_GetStatus");
    public static readonly FT_ReadDelegate 
      FT_Read = CreateDelegate<
      FT_ReadDelegate>("FT_Read");
    public static readonly FT_ReadByteDelegate
      FT_ReadByte = CreateDelegate<
      FT_ReadByteDelegate>("FT_Read");

    private FTD2XX() { }

    public static FT_STATUS Write(FT_HANDLE handle, byte[] buffer) {
      uint bytesWritten;
      FT_STATUS status = FT_Write(handle, buffer, (uint)buffer.Length, 
        out bytesWritten);
      if (bytesWritten != buffer.Length)
        return FT_STATUS.FT_FAILED_TO_WRITE_DEVICE;
      else
        return status;
    }

    public static int BytesToRead(FT_HANDLE handle) {
      uint amountInRxQueue;
      uint amountInTxQueue;
      uint eventStatus;
      if (FT_GetStatus(handle, out amountInRxQueue, out amountInTxQueue,
        out eventStatus) == FT_STATUS.FT_OK) {
        return (int)amountInRxQueue;
      } else {
        return 0;
      }
    }

    public static byte ReadByte(FT_HANDLE handle) {
      byte buffer;
      uint bytesReturned;
      FT_STATUS status = FT_ReadByte(handle, out buffer, 1, out bytesReturned);
      if (status != FT_STATUS.FT_OK || bytesReturned != 1)
        throw new InvalidOperationException();
      return buffer;
    }

    public static void Read(FT_HANDLE handle, byte[] buffer) {
      uint bytesReturned;
      FT_STATUS status = 
        FT_Read(handle, buffer, (uint)buffer.Length, out bytesReturned);
      if (status != FT_STATUS.FT_OK || bytesReturned != buffer.Length)
        throw new InvalidOperationException();
    }

    private static string GetDllName() {
      if (OperatingSystem.IsUnix)
        return "libftd2xx.so";
      else
        return "ftd2xx.dll";
    }

    private static T CreateDelegate<T>(string entryPoint)
      where T : class {
      DllImportAttribute attribute = new DllImportAttribute(GetDllName());
      attribute.CallingConvention = CallingConvention.StdCall;
      attribute.PreserveSig = true;
      attribute.EntryPoint = entryPoint;
      T newDelegate;
      PInvokeDelegateFactory.CreateDelegate(attribute, out newDelegate);
      return newDelegate;
    }
  }
}
