/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2010-2012 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Security.AccessControl;
using Microsoft.Win32.SafeHandles;

namespace OpenHardwareMonitor.Hardware {
  internal class KernelDriver {

    private string id;

    private SafeFileHandle device; 
    
    public KernelDriver(string id) {
      this.id = id;
    }
   
    public bool Install(string path, out string errorMessage) {
      IntPtr manager = NativeMethods.OpenSCManager(null, null,
        ServiceControlManagerAccessRights.SC_MANAGER_ALL_ACCESS);

      if (manager == IntPtr.Zero) {
        errorMessage = "OpenSCManager returned zero.";
        return false;
      }

      IntPtr service = NativeMethods.CreateService(manager, id, id,
        ServiceAccessRights.SERVICE_ALL_ACCESS,
        ServiceType.SERVICE_KERNEL_DRIVER, StartType.SERVICE_DEMAND_START,
        ErrorControl.SERVICE_ERROR_NORMAL, path, null, null, null, null,
        null);

      if (service == IntPtr.Zero) {
        if (Marshal.GetHRForLastWin32Error() == ERROR_SERVICE_EXISTS) {
          errorMessage = "Service already exists";
          return false;
        } else {
          errorMessage = "CreateService returned the error: " +
            Marshal.GetExceptionForHR(Marshal.GetHRForLastWin32Error()).Message;
          NativeMethods.CloseServiceHandle(manager);
          return false;
        }
      }

      if (!NativeMethods.StartService(service, 0, null)) {
        if (Marshal.GetHRForLastWin32Error() != ERROR_SERVICE_ALREADY_RUNNING) {
          errorMessage = "StartService returned the error: " +
            Marshal.GetExceptionForHR(Marshal.GetHRForLastWin32Error()).Message;
          NativeMethods.CloseServiceHandle(service);
          NativeMethods.CloseServiceHandle(manager);
          return false;
        }
      }

      NativeMethods.CloseServiceHandle(service);
      NativeMethods.CloseServiceHandle(manager);
      
      try {
        // restrict the driver access to system (SY) and builtin admins (BA)
        // TODO: replace with a call to IoCreateDeviceSecure in the driver
        FileSecurity fileSecurity = File.GetAccessControl(@"\\.\" + id);
        fileSecurity.SetSecurityDescriptorSddlForm(
          "O:BAG:SYD:(A;;FA;;;SY)(A;;FA;;;BA)");
        File.SetAccessControl(@"\\.\" + id, fileSecurity);
      } catch { }

      errorMessage = null;
      return true;
    }

    public bool Open() {
      device = new SafeFileHandle(NativeMethods.CreateFile(@"\\.\" + id,
        FileAccess.GENERIC_READ | FileAccess.GENERIC_WRITE, 0, IntPtr.Zero,
        CreationDisposition.OPEN_EXISTING, FileAttributes.FILE_ATTRIBUTE_NORMAL,
        IntPtr.Zero), true);

      if (device.IsInvalid) {
        device.Close();
        device.Dispose();
        device = null;
      }

      return device != null;
    }

    public bool IsOpen {
      get { return device != null; }
    }

    public bool DeviceIOControl(IOControlCode ioControlCode, object inBuffer) {
      if (device == null)
        return false;

      uint bytesReturned;
      bool b = NativeMethods.DeviceIoControl(device, ioControlCode,
        inBuffer, inBuffer == null ? 0 : (uint)Marshal.SizeOf(inBuffer),
        null, 0, out bytesReturned, IntPtr.Zero);
      return b;
    }

    public bool DeviceIOControl<T>(IOControlCode ioControlCode, object inBuffer, 
      ref T outBuffer) 
    {
      if (device == null)
        return false;

      object boxedOutBuffer = outBuffer;
      uint bytesReturned;
      bool b = NativeMethods.DeviceIoControl(device, ioControlCode,
        inBuffer, inBuffer == null ? 0 : (uint)Marshal.SizeOf(inBuffer),
        boxedOutBuffer, (uint)Marshal.SizeOf(boxedOutBuffer),
        out bytesReturned, IntPtr.Zero);
      outBuffer = (T)boxedOutBuffer;
      return b;
    }

    public void Close() {
      if (device != null) {
        device.Close();
        device.Dispose();
        device = null;
      }
    }

    public bool Delete() {
      IntPtr manager = NativeMethods.OpenSCManager(null, null,
      ServiceControlManagerAccessRights.SC_MANAGER_ALL_ACCESS);

      if (manager == IntPtr.Zero)
        return false;      

      IntPtr service = NativeMethods.OpenService(manager, id,
        ServiceAccessRights.SERVICE_ALL_ACCESS);

      if (service == IntPtr.Zero)
        return true;

      ServiceStatus status = new ServiceStatus();
      NativeMethods.ControlService(service, ServiceControl.SERVICE_CONTROL_STOP, 
        ref status);

      NativeMethods.DeleteService(service);

      NativeMethods.CloseServiceHandle(service);
      NativeMethods.CloseServiceHandle(manager);
      
      return true;
    }

    private enum ServiceAccessRights : uint {
      SERVICE_ALL_ACCESS = 0xF01FF
    }

    private enum ServiceControlManagerAccessRights : uint {
      SC_MANAGER_ALL_ACCESS = 0xF003F
    }

    private enum ServiceType : uint {
      SERVICE_KERNEL_DRIVER = 1,
      SERVICE_FILE_SYSTEM_DRIVER = 2
    }

    private enum StartType : uint {
      SERVICE_BOOT_START = 0,
      SERVICE_SYSTEM_START = 1,
      SERVICE_AUTO_START = 2,
      SERVICE_DEMAND_START = 3,
      SERVICE_DISABLED = 4
    }

    private enum ErrorControl : uint {
      SERVICE_ERROR_IGNORE = 0,
      SERVICE_ERROR_NORMAL = 1,
      SERVICE_ERROR_SEVERE = 2,
      SERVICE_ERROR_CRITICAL = 3
    }

    private enum ServiceControl : uint {
      SERVICE_CONTROL_STOP = 1,
      SERVICE_CONTROL_PAUSE = 2,
      SERVICE_CONTROL_CONTINUE = 3,
      SERVICE_CONTROL_INTERROGATE = 4,
      SERVICE_CONTROL_SHUTDOWN = 5,
      SERVICE_CONTROL_PARAMCHANGE = 6,
      SERVICE_CONTROL_NETBINDADD = 7,
      SERVICE_CONTROL_NETBINDREMOVE = 8,
      SERVICE_CONTROL_NETBINDENABLE = 9,
      SERVICE_CONTROL_NETBINDDISABLE = 10,
      SERVICE_CONTROL_DEVICEEVENT = 11,
      SERVICE_CONTROL_HARDWAREPROFILECHANGE = 12,
      SERVICE_CONTROL_POWEREVENT = 13,
      SERVICE_CONTROL_SESSIONCHANGE = 14
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    private struct ServiceStatus {
      public uint dwServiceType;
      public uint dwCurrentState;
      public uint dwControlsAccepted;
      public uint dwWin32ExitCode;
      public uint dwServiceSpecificExitCode;
      public uint dwCheckPoint;
      public uint dwWaitHint;
    }

    private enum FileAccess : uint {
      GENERIC_READ = 0x80000000,
      GENERIC_WRITE = 0x40000000
    }

    private enum CreationDisposition : uint {
      CREATE_NEW = 1,
      CREATE_ALWAYS = 2,
      OPEN_EXISTING = 3,
      OPEN_ALWAYS = 4,
      TRUNCATE_EXISTING = 5
    }

    private enum FileAttributes : uint {
      FILE_ATTRIBUTE_NORMAL = 0x80
    }

    private const int
      ERROR_SERVICE_EXISTS = unchecked((int)0x80070431),
      ERROR_SERVICE_ALREADY_RUNNING = unchecked((int)0x80070420);

    private static class NativeMethods {
      private const string ADVAPI = "advapi32.dll";
      private const string KERNEL = "kernel32.dll";

      [DllImport(ADVAPI, CallingConvention = CallingConvention.Winapi)]
      public static extern IntPtr OpenSCManager(string machineName,
        string databaseName, ServiceControlManagerAccessRights dwAccess);

      [DllImport(ADVAPI, CallingConvention = CallingConvention.Winapi)]
      [return: MarshalAs(UnmanagedType.Bool)]
      public static extern bool CloseServiceHandle(IntPtr hSCObject);

      [DllImport(ADVAPI, CallingConvention = CallingConvention.Winapi,
        SetLastError = true)]
      public static extern IntPtr CreateService(IntPtr hSCManager,
        string lpServiceName, string lpDisplayName, 
        ServiceAccessRights dwDesiredAccess, ServiceType dwServiceType,
        StartType dwStartType, ErrorControl dwErrorControl,
        string lpBinaryPathName, string lpLoadOrderGroup, string lpdwTagId,
        string lpDependencies, string lpServiceStartName, string lpPassword);

      [DllImport(ADVAPI, CallingConvention = CallingConvention.Winapi,
        SetLastError = true)]
      public static extern IntPtr OpenService(IntPtr hSCManager,
        string lpServiceName, ServiceAccessRights dwDesiredAccess);

      [DllImport(ADVAPI, CallingConvention = CallingConvention.Winapi,
        SetLastError = true)]
      [return: MarshalAs(UnmanagedType.Bool)]
      public static extern bool DeleteService(IntPtr hService);

      [DllImport(ADVAPI, CallingConvention = CallingConvention.Winapi,
        SetLastError = true)]
      [return: MarshalAs(UnmanagedType.Bool)]
      public static extern bool StartService(IntPtr hService, 
        uint dwNumServiceArgs, string[] lpServiceArgVectors);

      [DllImport(ADVAPI, CallingConvention = CallingConvention.Winapi,
        SetLastError = true)]
      [return: MarshalAs(UnmanagedType.Bool)]
      public static extern bool ControlService(IntPtr hService,
        ServiceControl dwControl, ref ServiceStatus lpServiceStatus);

      [DllImport(KERNEL, CallingConvention = CallingConvention.Winapi)]
      public static extern bool DeviceIoControl(SafeFileHandle device,
        IOControlCode ioControlCode, 
        [MarshalAs(UnmanagedType.AsAny)] [In] object inBuffer, 
        uint inBufferSize,
        [MarshalAs(UnmanagedType.AsAny)] [Out] object outBuffer,
        uint nOutBufferSize, out uint bytesReturned, IntPtr overlapped);

      [DllImport(KERNEL, CallingConvention = CallingConvention.Winapi, 
        SetLastError = true)]
      public static extern IntPtr CreateFile(string lpFileName,
        FileAccess dwDesiredAccess, uint dwShareMode, 
        IntPtr lpSecurityAttributes, CreationDisposition dwCreationDisposition, 
        FileAttributes dwFlagsAndAttributes, IntPtr hTemplateFile);
    }
  }
}
