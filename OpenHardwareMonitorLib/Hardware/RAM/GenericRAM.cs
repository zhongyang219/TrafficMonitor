/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2012 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System.Runtime.InteropServices;

namespace OpenHardwareMonitor.Hardware.RAM {
  internal class GenericRAM : Hardware {

    private Sensor loadSensor;
    private Sensor usedMemory;
    private Sensor availableMemory;

    public GenericRAM(string name, ISettings settings)
      : base(name, new Identifier("ram"), settings)
    {   
      loadSensor = new Sensor("Memory", 0, SensorType.Load, this, settings);
      ActivateSensor(loadSensor);

      usedMemory = new Sensor("Used Memory", 0, SensorType.Data, this,
        settings);
      ActivateSensor(usedMemory);

      availableMemory = new Sensor("Available Memory", 1, SensorType.Data, this, 
        settings);
      ActivateSensor(availableMemory);
    }

    public override HardwareType HardwareType {
      get {
        return HardwareType.RAM;
      }
    }

    public override void Update() {
      NativeMethods.MemoryStatusEx status = new NativeMethods.MemoryStatusEx();
      status.Length = checked((uint)Marshal.SizeOf(
          typeof(NativeMethods.MemoryStatusEx)));

      if (!NativeMethods.GlobalMemoryStatusEx(ref status))
        return;

      loadSensor.Value = 100.0f -
        (100.0f * status.AvailablePhysicalMemory) /
        status.TotalPhysicalMemory;

      usedMemory.Value = (float)(status.TotalPhysicalMemory 
        - status.AvailablePhysicalMemory) / (1024 * 1024 * 1024);

      availableMemory.Value = (float)status.AvailablePhysicalMemory /
        (1024 * 1024 * 1024);
    }

    private class NativeMethods {
      [StructLayout(LayoutKind.Sequential)]
      public struct MemoryStatusEx {
        public uint Length;
        public uint MemoryLoad;
        public ulong TotalPhysicalMemory;
        public ulong AvailablePhysicalMemory;
        public ulong TotalPageFile;
        public ulong AvailPageFile;
        public ulong TotalVirtual;
        public ulong AvailVirtual;
        public ulong AvailExtendedVirtual;
      }

      [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
      [return: MarshalAs(UnmanagedType.Bool)]
      internal static extern bool GlobalMemoryStatusEx(
        ref NativeMethods.MemoryStatusEx buffer);
    }
  }
}
