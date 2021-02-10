/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2010 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Runtime.InteropServices;
using System.Reflection;

namespace OpenHardwareMonitor.Hardware {
  internal static class Opcode {
    
    private static IntPtr codeBuffer;
    private static ulong size;

    public static void Open() {              
      byte[] rdtscCode;
      byte[] cpuidCode;
      if (IntPtr.Size == 4) {
        rdtscCode = RDTSC_32;
        cpuidCode = CPUID_32;
      } else {
        rdtscCode = RDTSC_64;
        
        if (OperatingSystem.IsUnix) { // Unix
          cpuidCode = CPUID_64_LINUX;
        } else { // Windows
          cpuidCode = CPUID_64_WINDOWS;
        }
      }
      
      size = (ulong)(rdtscCode.Length + cpuidCode.Length);

      if (OperatingSystem.IsUnix) { // Unix   
        Assembly assembly = 
          Assembly.Load("Mono.Posix, Version=2.0.0.0, Culture=neutral, " +
          "PublicKeyToken=0738eb9f132ed756");

        Type syscall = assembly.GetType("Mono.Unix.Native.Syscall");
        MethodInfo mmap = syscall.GetMethod("mmap");

        Type mmapProts = assembly.GetType("Mono.Unix.Native.MmapProts");
        object mmapProtsParam = Enum.ToObject(mmapProts,
          (int)mmapProts.GetField("PROT_READ").GetValue(null) |
          (int)mmapProts.GetField("PROT_WRITE").GetValue(null) |
          (int)mmapProts.GetField("PROT_EXEC").GetValue(null));

        Type mmapFlags = assembly.GetType("Mono.Unix.Native.MmapFlags");
        object mmapFlagsParam = Enum.ToObject(mmapFlags,
          (int)mmapFlags.GetField("MAP_ANONYMOUS").GetValue(null) |
          (int)mmapFlags.GetField("MAP_PRIVATE").GetValue(null));
        
        codeBuffer = (IntPtr)mmap.Invoke(null, new object[] { IntPtr.Zero, 
          size, mmapProtsParam, mmapFlagsParam, -1, 0 });        
      } else { // Windows
        codeBuffer = NativeMethods.VirtualAlloc(IntPtr.Zero,
          (UIntPtr)size, AllocationType.COMMIT | AllocationType.RESERVE, 
          MemoryProtection.EXECUTE_READWRITE);
      }

      Marshal.Copy(rdtscCode, 0, codeBuffer, rdtscCode.Length);

      Rdtsc = Marshal.GetDelegateForFunctionPointer(
        codeBuffer, typeof(RdtscDelegate)) as RdtscDelegate;

      IntPtr cpuidAddress = (IntPtr)((long)codeBuffer + rdtscCode.Length);
      Marshal.Copy(cpuidCode, 0, cpuidAddress, cpuidCode.Length);

      Cpuid = Marshal.GetDelegateForFunctionPointer(
        cpuidAddress, typeof(CpuidDelegate)) as CpuidDelegate;         
    }

    public static void Close() {
      Rdtsc = null;
      Cpuid = null;
      
      if (OperatingSystem.IsUnix) { // Unix
        Assembly assembly =
          Assembly.Load("Mono.Posix, Version=2.0.0.0, Culture=neutral, " +
          "PublicKeyToken=0738eb9f132ed756");

        Type syscall = assembly.GetType("Mono.Unix.Native.Syscall");
        MethodInfo munmap = syscall.GetMethod("munmap");
        munmap.Invoke(null, new object[] { codeBuffer, size });

      } else { // Windows
        NativeMethods.VirtualFree(codeBuffer, UIntPtr.Zero, 
          FreeType.RELEASE);        
      }
    }

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate ulong RdtscDelegate();

    public static RdtscDelegate Rdtsc;

    // unsigned __int64 __stdcall rdtsc() {
    //   return __rdtsc();
    // }

    private static readonly byte[] RDTSC_32 = new byte[] {
      0x0F, 0x31,                     // rdtsc   
      0xC3                            // ret  
    };

    private static readonly byte[] RDTSC_64 = new byte[] {
      0x0F, 0x31,                     // rdtsc  
      0x48, 0xC1, 0xE2, 0x20,         // shl rdx, 20h  
      0x48, 0x0B, 0xC2,               // or rax, rdx  
      0xC3                            // ret  
    };
    
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool CpuidDelegate(uint index, uint ecxValue,
      out uint eax, out uint ebx, out uint ecx, out uint edx);

    public static CpuidDelegate Cpuid;


    // void __stdcall cpuidex(unsigned int index, unsigned int ecxValue, 
    //   unsigned int* eax, unsigned int* ebx, unsigned int* ecx, 
    //   unsigned int* edx)
    // {
    //   int info[4];	
    //   __cpuidex(info, index, ecxValue);
    //   *eax = info[0];
    //   *ebx = info[1];
    //   *ecx = info[2];
    //   *edx = info[3];
    // }

    private static readonly byte[] CPUID_32 = new byte[] {
      0x55,                           // push ebp  
      0x8B, 0xEC,                     // mov ebp, esp  
      0x83, 0xEC, 0x10,               // sub esp, 10h  
      0x8B, 0x45, 0x08,               // mov eax, dword ptr [ebp+8]  
      0x8B, 0x4D, 0x0C,               // mov ecx, dword ptr [ebp+0Ch]  
      0x53,                           // push ebx  
      0x0F, 0xA2,                     // cpuid  
      0x56,                           // push esi  
      0x8D, 0x75, 0xF0,               // lea esi, [info]  
      0x89, 0x06,                     // mov dword ptr [esi],eax  
      0x8B, 0x45, 0x10,               // mov eax, dword ptr [eax]  
      0x89, 0x5E, 0x04,               // mov dword ptr [esi+4], ebx  
      0x89, 0x4E, 0x08,               // mov dword ptr [esi+8], ecx  
      0x89, 0x56, 0x0C,               // mov dword ptr [esi+0Ch], edx  
      0x8B, 0x4D, 0xF0,               // mov ecx, dword ptr [info]  
      0x89, 0x08,                     // mov dword ptr [eax], ecx  
      0x8B, 0x45, 0x14,               // mov eax, dword ptr [ebx]  
      0x8B, 0x4D, 0xF4,               // mov ecx, dword ptr [ebp-0Ch]  
      0x89, 0x08,                     // mov dword ptr [eax], ecx  
      0x8B, 0x45, 0x18,               // mov eax, dword ptr [ecx]  
      0x8B, 0x4D, 0xF8,               // mov ecx, dword ptr [ebp-8]  
      0x89, 0x08,                     // mov dword ptr [eax], ecx  
      0x8B, 0x45, 0x1C,               // mov eax, dword ptr [edx]  
      0x8B, 0x4D, 0xFC,               // mov ecx, dword ptr [ebp-4]  
      0x5E,                           // pop esi  
      0x89, 0x08,                     // mov dword ptr [eax], ecx  
      0x5B,                           // pop ebx  
      0xC9,                           // leave  
      0xC2, 0x18, 0x00                // ret 18h  
    };
             
    private static readonly byte[] CPUID_64_WINDOWS = new byte[] {
      0x48, 0x89, 0x5C, 0x24, 0x08,   // mov qword ptr [rsp+8], rbx  
      0x8B, 0xC1,                     // mov eax, ecx  
      0x8B, 0xCA,                     // mov ecx, edx        
      0x0F, 0xA2,                     // cpuid        
      0x41, 0x89, 0x00,               // mov dword ptr [r8], eax        
      0x48, 0x8B, 0x44, 0x24, 0x28,   // mov rax, qword ptr [rsp+28h]       
      0x41, 0x89, 0x19,               // mov dword ptr [r9], ebx        
      0x48, 0x8B, 0x5C, 0x24, 0x08,   // mov rbx, qword ptr [rsp+8]      
      0x89, 0x08,                     // mov dword ptr [rax], ecx        
      0x48, 0x8B, 0x44, 0x24, 0x30,   // mov rax, qword ptr [rsp+30h]  
      0x89, 0x10,                     // mov dword ptr [rax], edx  
      0xC3                            // ret  
    };
    
    private static readonly byte[] CPUID_64_LINUX = new byte[] {
      0x49, 0x89, 0xD2,               // mov r10, rdx
      0x49, 0x89, 0xCB,               // mov r11, rcx
      0x53,                           // push rbx
      0x89, 0xF8,                     // mov eax, edi
      0x89, 0xF1,                     // mov ecx, esi
      0x0F, 0xA2,                     // cpuid
      0x41, 0x89, 0x02,               // mov dword ptr [r10], eax
      0x41, 0x89, 0x1B,               // mov dword ptr [r11], ebx
      0x41, 0x89, 0x08,               // mov dword ptr [r8], ecx
      0x41, 0x89, 0x11,               // mov dword ptr [r9], edx
      0x5B,                           // pop rbx
      0xC3,                           // ret
    };

    public static bool CpuidTx(uint index, uint ecxValue, 
      out uint eax, out uint ebx, out uint ecx, out uint edx, 
      GroupAffinity affinity) 
    {      
      var previousAffinity = ThreadAffinity.Set(affinity);

      if (previousAffinity == GroupAffinity.Undefined) {
        eax = ebx = ecx = edx = 0;
        return false;
      } 

      Cpuid(index, ecxValue, out eax, out ebx, out ecx, out edx);

      ThreadAffinity.Set(previousAffinity);      
      return true;
    }
    
    [Flags()]
    public enum AllocationType : uint {
      COMMIT = 0x1000,
      RESERVE = 0x2000,
      RESET = 0x80000,
      LARGE_PAGES = 0x20000000,
      PHYSICAL = 0x400000,
      TOP_DOWN = 0x100000,
      WRITE_WATCH = 0x200000
    }

    [Flags()]
    public enum MemoryProtection : uint {
      EXECUTE = 0x10,
      EXECUTE_READ = 0x20,
      EXECUTE_READWRITE = 0x40,
      EXECUTE_WRITECOPY = 0x80,
      NOACCESS = 0x01,
      READONLY = 0x02,
      READWRITE = 0x04,
      WRITECOPY = 0x08,
      GUARD = 0x100,
      NOCACHE = 0x200,
      WRITECOMBINE = 0x400
    }

    [Flags]
    public enum FreeType {
      DECOMMIT = 0x4000,
      RELEASE = 0x8000
    }

    private static class NativeMethods {      
      private const string KERNEL = "kernel32.dll";

      [DllImport(KERNEL, CallingConvention = CallingConvention.Winapi)]
      public static extern IntPtr VirtualAlloc(IntPtr lpAddress, UIntPtr dwSize,
        AllocationType flAllocationType, MemoryProtection flProtect);

      [DllImport(KERNEL, CallingConvention = CallingConvention.Winapi)]
      public static extern bool VirtualFree(IntPtr lpAddress, UIntPtr dwSize,
        FreeType dwFreeType);                 
    }
  }
}
