/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2010-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Runtime.InteropServices;

namespace OpenHardwareMonitor.Hardware {

  internal static class ThreadAffinity {

    static ThreadAffinity() {
      ProcessorGroupCount = GetProcessorGroupCount();
    }

    private static int GetProcessorGroupCount() {
      if (OperatingSystem.IsUnix)
        return 1;

      try {
        return NativeMethods.GetActiveProcessorGroupCount();
      } catch {
        return 1;
      }
    }

    public static int ProcessorGroupCount { get; }

    public static bool IsValid(GroupAffinity affinity) {
      if (OperatingSystem.IsUnix) {
        if (affinity.Group > 0)
          return false;
      }

      try {
        var previous = Set(affinity);
        if (previous == GroupAffinity.Undefined)
          return false;
        Set(previous);
        return true;
      } catch {
        return false;
      }      
    }

    /// <summary>
    /// Sets the processor group affinity for the current thread.
    /// </summary>
    /// <param name="affinity">The processor group affinity.</param>
    /// <returns>The previous processor group affinity.</returns>
    public static GroupAffinity Set(GroupAffinity affinity) {
      if (affinity == GroupAffinity.Undefined)
        return GroupAffinity.Undefined;

      if (OperatingSystem.IsUnix) {
        if (affinity.Group > 0)
          throw new ArgumentOutOfRangeException("affinity.Group");

        ulong result = 0;
        if (NativeMethods.sched_getaffinity(0, (IntPtr)8, ref result) != 0)
          return GroupAffinity.Undefined;

        ulong mask = affinity.Mask;
        if (NativeMethods.sched_setaffinity(0, (IntPtr)8, ref mask) != 0)
          return GroupAffinity.Undefined;

        return new GroupAffinity(0, result);
      } else {
        UIntPtr uIntPtrMask;
        try {
          uIntPtrMask = (UIntPtr)affinity.Mask;
        } catch (OverflowException) {
          throw new ArgumentOutOfRangeException("affinity.Mask");
        }

        var groupAffinity = new NativeMethods.GROUP_AFFINITY {
          Group = affinity.Group,
          Mask = uIntPtrMask
        };

        var currentThread = NativeMethods.GetCurrentThread();

        try {
          if (NativeMethods.SetThreadGroupAffinity(currentThread, 
            ref groupAffinity, out var previousGroupAffinity)) 
          {
            return new GroupAffinity(previousGroupAffinity.Group,
              (ulong)previousGroupAffinity.Mask);
          } else {
            return GroupAffinity.Undefined;
          }
        } catch (EntryPointNotFoundException) {
          if (affinity.Group > 0)
            throw new ArgumentOutOfRangeException("affinity.Group");

          var previous = (ulong)NativeMethods.SetThreadAffinityMask(
            currentThread, uIntPtrMask);

          return new GroupAffinity(0, previous);
        }        
      }
    }

    private static class NativeMethods {      
      private const string KERNEL = "kernel32.dll";

      [DllImport(KERNEL, CallingConvention = CallingConvention.Winapi)]
      public static extern UIntPtr
        SetThreadAffinityMask(IntPtr handle, UIntPtr mask);

      [DllImport(KERNEL, CallingConvention = CallingConvention.Winapi)]
      public static extern IntPtr GetCurrentThread();

      [DllImport(KERNEL, CallingConvention = CallingConvention.Winapi)]
      public static extern ushort GetActiveProcessorGroupCount();

      [StructLayout(LayoutKind.Sequential, Pack = 4)]
      public struct GROUP_AFFINITY {
        public UIntPtr Mask;
        [MarshalAs(UnmanagedType.U2)]
        public ushort Group;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3, 
          ArraySubType = UnmanagedType.U2)]
        public ushort[] Reserved;
      }

      [DllImport(KERNEL, CallingConvention = CallingConvention.Winapi)]
      public static extern bool SetThreadGroupAffinity(
          IntPtr thread,
          ref GROUP_AFFINITY groupAffinity,
          out GROUP_AFFINITY previousGroupAffinity);

      private const string LIBC = "libc";
      
      [DllImport(LIBC)]
      public static extern int sched_getaffinity(int pid, IntPtr maskSize,
        ref ulong mask);
      
      [DllImport(LIBC)]
      public static extern int sched_setaffinity(int pid, IntPtr maskSize,
        ref ulong mask);  
    }  
  }
}

