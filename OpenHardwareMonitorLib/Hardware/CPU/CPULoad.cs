/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2011 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Runtime.InteropServices;

namespace OpenHardwareMonitor.Hardware.CPU {
  internal class CPULoad {

    [StructLayout(LayoutKind.Sequential)]
    protected struct SystemProcessorPerformanceInformation {
      public long IdleTime;
      public long KernelTime;
      public long UserTime;
      public long Reserved0;
      public long Reserved1;
      public ulong Reserved2;
    }

    protected enum SystemInformationClass {
      SystemBasicInformation = 0,
      SystemCpuInformation = 1,
      SystemPerformanceInformation = 2,
      SystemTimeOfDayInformation = 3,
      SystemProcessInformation = 5,
      SystemProcessorPerformanceInformation = 8
    }

    private readonly CPUID[][] cpuid;

    private long[] idleTimes;
    private long[] totalTimes;

    private float totalLoad;
    private readonly float[] coreLoads;

    private readonly bool available;

    private static bool GetTimes(out long[] idle, out long[] total) {      
      SystemProcessorPerformanceInformation[] informations = new
        SystemProcessorPerformanceInformation[64];

      int size = Marshal.SizeOf(typeof(SystemProcessorPerformanceInformation));

      idle = null;
      total = null;

      IntPtr returnLength;
      if (NativeMethods.NtQuerySystemInformation(
        SystemInformationClass.SystemProcessorPerformanceInformation,
        informations, informations.Length * size, out returnLength) != 0)
        return false;

      idle = new long[(int)returnLength / size];
      total = new long[(int)returnLength / size];

      for (int i = 0; i < idle.Length; i++) {
        idle[i] = informations[i].IdleTime;
        total[i] = informations[i].KernelTime + informations[i].UserTime;
      }

      return true;
    }

    public CPULoad(CPUID[][] cpuid) {
      this.cpuid = cpuid;
      this.coreLoads = new float[cpuid.Length];         
      this.totalLoad = 0;
      try {
        GetTimes(out idleTimes, out totalTimes);
      } catch (Exception) {
        this.idleTimes = null;
        this.totalTimes = null;
      }
      if (idleTimes != null)
        available = true;
    }

    public bool IsAvailable {
      get { return available; }
    }

    public float GetTotalLoad() {
      return totalLoad;
    }

    public float GetCoreLoad(int core) {
      return coreLoads[core];
    }

    public void Update() {
      if (this.idleTimes == null)
        return;

      long[] newIdleTimes;
      long[] newTotalTimes;

      if (!GetTimes(out newIdleTimes, out newTotalTimes))
        return;

      for (int i = 0; i < Math.Min(newTotalTimes.Length, totalTimes.Length); i++) 
        if (newTotalTimes[i] - this.totalTimes[i] < 100000)
          return;

      if (newIdleTimes == null || newTotalTimes == null)
        return;

      float total = 0;
      int count = 0;
      for (int i = 0; i < cpuid.Length; i++) {
        float value = 0;
        for (int j = 0; j < cpuid[i].Length; j++) {
          long index = cpuid[i][j].Thread;
          if (index < newIdleTimes.Length && index < totalTimes.Length) {
            float idle = 
              (float)(newIdleTimes[index] - this.idleTimes[index]) /
              (float)(newTotalTimes[index] - this.totalTimes[index]);
            value += idle;
            total += idle;
            count++;
          }
        }
        value = 1.0f - value / cpuid[i].Length;
        value = value < 0 ? 0 : value;
        coreLoads[i] = value * 100;
      }
      if (count > 0) {
        total = 1.0f - total / count;
        total = total < 0 ? 0 : total;
      } else {
        total = 0;
      }
      this.totalLoad = total * 100;

      this.totalTimes = newTotalTimes;
      this.idleTimes = newIdleTimes;
    }

    protected static class NativeMethods {

      [DllImport("ntdll.dll")]
      public static extern int NtQuerySystemInformation(
        SystemInformationClass informationClass,
        [Out] SystemProcessorPerformanceInformation[] informations,
        int structSize, out IntPtr returnLength);
    }
  }
}
