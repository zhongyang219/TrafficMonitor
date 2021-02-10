/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2010-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.AccessControl;
using System.Threading;
using System.Text;

namespace OpenHardwareMonitor.Hardware {
  internal static class Ring0 {

    private static KernelDriver driver;
    private static string fileName;
    private static Mutex isaBusMutex;
    private static Mutex pciBusMutex;
    private static readonly StringBuilder report = new StringBuilder();

    private const uint OLS_TYPE = 40000;
    private static IOControlCode
      IOCTL_OLS_GET_REFCOUNT = new IOControlCode(OLS_TYPE, 0x801,
        IOControlCode.Access.Any),
      IOCTL_OLS_GET_DRIVER_VERSION = new IOControlCode(OLS_TYPE, 0x800,
        IOControlCode.Access.Any),
      IOCTL_OLS_READ_MSR = new IOControlCode(OLS_TYPE, 0x821,
        IOControlCode.Access.Any),
      IOCTL_OLS_WRITE_MSR = new IOControlCode(OLS_TYPE, 0x822, 
        IOControlCode.Access.Any),
      IOCTL_OLS_READ_IO_PORT_BYTE = new IOControlCode(OLS_TYPE, 0x833,
        IOControlCode.Access.Read),
      IOCTL_OLS_WRITE_IO_PORT_BYTE = new IOControlCode(OLS_TYPE, 0x836, 
        IOControlCode.Access.Write),
      IOCTL_OLS_READ_PCI_CONFIG = new IOControlCode(OLS_TYPE, 0x851, 
        IOControlCode.Access.Read),
      IOCTL_OLS_WRITE_PCI_CONFIG = new IOControlCode(OLS_TYPE, 0x852,
        IOControlCode.Access.Write),
      IOCTL_OLS_READ_MEMORY = new IOControlCode(OLS_TYPE, 0x841,
        IOControlCode.Access.Read);

    private static Assembly GetAssembly() {
      return typeof(Ring0).Assembly;
    }

    private static string GetTempFileName() {
      
      // try to create one in the application folder
      string location = GetAssembly().Location;
      if (!string.IsNullOrEmpty(location)) {        
        try {
          string fileName = Path.ChangeExtension(location, ".sys");
          using (FileStream stream = File.Create(fileName)) {
            return fileName;
          }
        } catch (Exception) { }
      }

      // if this failed, try to get a file in the temporary folder
      try {
        return Path.GetTempFileName();        
      } catch (IOException) { 
          // some I/O exception
      } 
      catch (UnauthorizedAccessException) { 
        // we do not have the right to create a file in the temp folder
      }
      catch (NotSupportedException) {
        // invalid path format of the TMP system environment variable
      }
     
      return null;
    }

    private static bool ExtractDriver(string fileName) {
      string resourceName = "OpenHardwareMonitor.Hardware." +
        (OperatingSystem.Is64BitOperatingSystem ? "WinRing0x64.sys" : 
        "WinRing0.sys");

      string[] names = GetAssembly().GetManifestResourceNames();
      byte[] buffer = null;
      for (int i = 0; i < names.Length; i++) {
        if (names[i].Replace('\\', '.') == resourceName) {
          using (Stream stream = GetAssembly().
            GetManifestResourceStream(names[i])) 
          {
              buffer = new byte[stream.Length];
              stream.Read(buffer, 0, buffer.Length);
          }
        }
      }

      if (buffer == null)
        return false;

      try {
        using (FileStream target = new FileStream(fileName, FileMode.Create)) {
          target.Write(buffer, 0, buffer.Length);
          target.Flush();
        }
      } catch (IOException) { 
        // for example there is not enough space on the disk
        return false; 
      }

      // make sure the file is actually writen to the file system
      for (int i = 0; i < 20; i++) {
        try {
          if (File.Exists(fileName) &&
            new FileInfo(fileName).Length == buffer.Length) 
          {
            return true;
          }
          Thread.Sleep(100);
        } catch (IOException) {
          Thread.Sleep(10);
        }
      }
      
      // file still has not the right size, something is wrong
      return false;
    }

    public static void Open() {
      // no implementation for unix systems
      if (OperatingSystem.IsUnix)
        return;  
      
      if (driver != null)
        return;

      // clear the current report
      report.Length = 0;
     
      driver = new KernelDriver("WinRing0_1_2_0");
      driver.Open();

      if (!driver.IsOpen) {
        // driver is not loaded, try to install and open

        fileName = GetTempFileName();
        if (fileName != null && ExtractDriver(fileName)) {
          string installError;
          if (driver.Install(fileName, out installError)) {
            driver.Open();

            if (!driver.IsOpen) {
              driver.Delete();
              report.AppendLine("Status: Opening driver failed after install");
            }
          } else {
            string errorFirstInstall = installError;
   
            // install failed, try to delete and reinstall
            driver.Delete();

            // wait a short moment to give the OS a chance to remove the driver
            Thread.Sleep(2000);

            string errorSecondInstall;
            if (driver.Install(fileName, out errorSecondInstall)) {
              driver.Open();

              if (!driver.IsOpen) {
                driver.Delete();
                report.AppendLine(
                  "Status: Opening driver failed after reinstall");
              }
            } else {
              report.AppendLine("Status: Installing driver \"" +
                fileName + "\" failed" +
                (File.Exists(fileName) ? " and file exists" : ""));
              report.AppendLine("First Exception: " + errorFirstInstall);
              report.AppendLine("Second Exception: " + errorSecondInstall);
            }
          }
        } else {
          report.AppendLine("Status: Extracting driver failed");
        }

        try {
          // try to delte the driver file
          if (File.Exists(fileName))
            File.Delete(fileName);
          fileName = null;
        } catch (IOException) { } 
          catch (UnauthorizedAccessException) { }
      }

      if (!driver.IsOpen) 
        driver = null;

      string isaMutexName = "Global\\Access_ISABUS.HTP.Method";
      try {
        isaBusMutex = new Mutex(false, isaMutexName);
      } catch (UnauthorizedAccessException) {
        try {
          isaBusMutex = Mutex.OpenExisting(isaMutexName, MutexRights.Synchronize);
        } catch { }
      }

      string pciMutexName = "Global\\Access_PCI";
      try {
        pciBusMutex = new Mutex(false, pciMutexName);
      } catch (UnauthorizedAccessException) {
        try {
          pciBusMutex = Mutex.OpenExisting(pciMutexName, MutexRights.Synchronize);
        } catch { }
      }
    }

    public static bool IsOpen {
      get { return driver != null; }
    }

    public static void Close() {
      if (driver == null)
        return;

      uint refCount = 0;
      driver.DeviceIOControl(IOCTL_OLS_GET_REFCOUNT, null, ref refCount);

      driver.Close();

      if (refCount <= 1)
        driver.Delete();

      driver = null;

      if (isaBusMutex != null) {
        isaBusMutex.Close();
        isaBusMutex = null;
      }

      if (pciBusMutex != null) {
        pciBusMutex.Close();
        pciBusMutex = null;
      }

      // try to delete temporary driver file again if failed during open
      if (fileName != null && File.Exists(fileName)) {
        try {
          File.Delete(fileName);
          fileName = null;
        } catch (IOException) { } 
          catch (UnauthorizedAccessException) { }
      }
    }

    public static string GetReport() {
      if (report.Length > 0) {
        StringBuilder r = new StringBuilder();
        r.AppendLine("Ring0");
        r.AppendLine();
        r.Append(report);
        r.AppendLine();
        return r.ToString();
      } else
        return null;
    }

    public static bool WaitIsaBusMutex(int millisecondsTimeout) {
      if (isaBusMutex == null)
        return true;
      try {
        return isaBusMutex.WaitOne(millisecondsTimeout, false);
      } catch (AbandonedMutexException) { return true; }
        catch (InvalidOperationException) { return false; }
    }

    public static void ReleaseIsaBusMutex() {
      if (isaBusMutex == null)
        return;
      isaBusMutex.ReleaseMutex();
    }

    public static bool WaitPciBusMutex(int millisecondsTimeout) {
      if (pciBusMutex == null)
        return true;
      try {
        return pciBusMutex.WaitOne(millisecondsTimeout, false);
      } catch (AbandonedMutexException) { return true; }
        catch (InvalidOperationException) { return false; }
    }

    public static void ReleasePciBusMutex() {
      if (pciBusMutex == null)
        return;
      pciBusMutex.ReleaseMutex();
    }

    public static bool Rdmsr(uint index, out uint eax, out uint edx) {
      if (driver == null) {
        eax = 0;
        edx = 0;
        return false;
      }

      ulong buffer = 0;
      bool result = driver.DeviceIOControl(IOCTL_OLS_READ_MSR, index,
        ref buffer);

      edx = (uint)((buffer >> 32) & 0xFFFFFFFF);
      eax = (uint)(buffer & 0xFFFFFFFF);
      return result;
    }

    public static bool RdmsrTx(uint index, out uint eax, out uint edx,
      GroupAffinity affinity) 
    {
      var previousAffinity = ThreadAffinity.Set(affinity);

      bool result = Rdmsr(index, out eax, out edx);

      ThreadAffinity.Set(previousAffinity);
      return result;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    private struct WrmsrInput {
      public uint Register;
      public ulong Value;
    }

    public static bool Wrmsr(uint index, uint eax, uint edx) {
      if (driver == null)
        return false;

      WrmsrInput input = new WrmsrInput();
      input.Register = index;
      input.Value = ((ulong)edx << 32) | eax;

      return driver.DeviceIOControl(IOCTL_OLS_WRITE_MSR, input);
    }

    public static byte ReadIoPort(uint port) {
      if (driver == null)
        return 0;

      uint value = 0;
      driver.DeviceIOControl(IOCTL_OLS_READ_IO_PORT_BYTE, port, ref value);

      return (byte)(value & 0xFF);
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    private struct WriteIoPortInput {
      public uint PortNumber;
      public byte Value;
    }

    public static void WriteIoPort(uint port, byte value) {
      if (driver == null)
        return;

      WriteIoPortInput input = new WriteIoPortInput();
      input.PortNumber = port;
      input.Value = value;

      driver.DeviceIOControl(IOCTL_OLS_WRITE_IO_PORT_BYTE, input);
    }

    public const uint InvalidPciAddress = 0xFFFFFFFF;

    public static uint GetPciAddress(byte bus, byte device, byte function) {
      return
        (uint)(((bus & 0xFF) << 8) | ((device & 0x1F) << 3) | (function & 7));
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    private struct ReadPciConfigInput {
      public uint PciAddress;
      public uint RegAddress;
    }

    public static bool ReadPciConfig(uint pciAddress, uint regAddress, 
      out uint value) 
    {
      if (driver == null || (regAddress & 3) != 0) {
        value = 0;
        return false;
      }

      ReadPciConfigInput input = new ReadPciConfigInput();
      input.PciAddress = pciAddress;
      input.RegAddress = regAddress;

      value = 0;
      return driver.DeviceIOControl(IOCTL_OLS_READ_PCI_CONFIG, input, 
        ref value);
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    private struct WritePciConfigInput {
      public uint PciAddress;
      public uint RegAddress;
      public uint Value;
    }

    public static bool WritePciConfig(uint pciAddress, uint regAddress, 
      uint value) 
    {
      if (driver == null || (regAddress & 3) != 0)
        return false;

      WritePciConfigInput input = new WritePciConfigInput();
      input.PciAddress = pciAddress;
      input.RegAddress = regAddress;
      input.Value = value;

      return driver.DeviceIOControl(IOCTL_OLS_WRITE_PCI_CONFIG, input);
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    private struct ReadMemoryInput {
      public ulong address;
      public uint unitSize;
      public uint count;
    }

    public static bool ReadMemory<T>(ulong address, ref T buffer) {
      if (driver == null) {
        return false;
      }

      ReadMemoryInput input = new ReadMemoryInput();
      input.address = address;
      input.unitSize = 1;
      input.count = (uint)Marshal.SizeOf(buffer);

      return driver.DeviceIOControl(IOCTL_OLS_READ_MEMORY, input,
        ref buffer);
    }
  }
}
