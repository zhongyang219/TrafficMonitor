/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text;

namespace OpenHardwareMonitor.Hardware.CPU {

  internal class CPUGroup : IGroup {
    private readonly List<GenericCPU> hardware = new List<GenericCPU>();

    private readonly CPUID[][][] threads;

    private static CPUID[][] GetProcessorThreads() {

      List<CPUID> threads = new List<CPUID>();
      for (int i = 0; i < ThreadAffinity.ProcessorGroupCount; i++) {
        for (int j = 0; j < 64; j++) {
          try {
            if (!ThreadAffinity.IsValid(GroupAffinity.Single((ushort)i, j)))
              continue;
            var cpuid = CPUID.Get(i, j);
            if (cpuid != null)
              threads.Add(cpuid);
          } catch (ArgumentOutOfRangeException) { 
          }
        }
      }

      SortedDictionary<uint, List<CPUID>> processors =
        new SortedDictionary<uint, List<CPUID>>();
      foreach (CPUID thread in threads) {
        List<CPUID> list;
        processors.TryGetValue(thread.ProcessorId, out list);
        if (list == null) {
          list = new List<CPUID>();
          processors.Add(thread.ProcessorId, list);
        }
        list.Add(thread);
      }

      CPUID[][] processorThreads = new CPUID[processors.Count][];
      int index = 0;
      foreach (List<CPUID> list in processors.Values) {
        processorThreads[index] = list.ToArray();
        index++;
      }
      return processorThreads;
    }

    private static CPUID[][] GroupThreadsByCore(IEnumerable<CPUID> threads) {

      SortedDictionary<uint, List<CPUID>> cores = 
        new SortedDictionary<uint, List<CPUID>>();
      foreach (CPUID thread in threads) {
        List<CPUID> coreList;
        cores.TryGetValue(thread.CoreId, out coreList);
        if (coreList == null) {
          coreList = new List<CPUID>();
          cores.Add(thread.CoreId, coreList);
        }
        coreList.Add(thread);
      }

      CPUID[][] coreThreads = new CPUID[cores.Count][];
      int index = 0;
      foreach (List<CPUID> list in cores.Values) {
        coreThreads[index] = list.ToArray();
        index++;
      }
      return coreThreads;
    }

    public CPUGroup(ISettings settings) {

      CPUID[][] processorThreads = GetProcessorThreads();
      this.threads = new CPUID[processorThreads.Length][][];

      int index = 0;
      foreach (CPUID[] threads in processorThreads) {
        if (threads.Length == 0)
          continue;
            
        CPUID[][] coreThreads = GroupThreadsByCore(threads);

        this.threads[index] = coreThreads;

        switch (threads[0].Vendor) {
          case Vendor.Intel:
            hardware.Add(new IntelCPU(index, coreThreads, settings));
            break;
          case Vendor.AMD:
            switch (threads[0].Family) {
              case 0x0F:
                hardware.Add(new AMD0FCPU(index, coreThreads, settings));
                break;
              case 0x10:
              case 0x11:
              case 0x12:
              case 0x14:
              case 0x15:
              case 0x16:
                hardware.Add(new AMD10CPU(index, coreThreads, settings));
                break;
              case 0x17:
              case 0x19:
                hardware.Add(new AMD17CPU(index, coreThreads, settings));
                break;
              default:
                hardware.Add(new GenericCPU(index, coreThreads, settings));
                break;
            } break;
          default:
            hardware.Add(new GenericCPU(index, coreThreads, settings));
            break;
        }

        index++;
      }
    }
    
    public IHardware[] Hardware {
      get {
        return hardware.ToArray();
      }
    }

    private static void AppendCpuidData(StringBuilder r, uint[,] data, 
      uint offset) 
    {
      for (int i = 0; i < data.GetLength(0); i++) {
        r.Append(" ");
        r.Append((i + offset).ToString("X8", CultureInfo.InvariantCulture));
        for (int j = 0; j < 4; j++) {
          r.Append("  ");
          r.Append(data[i, j].ToString("X8", CultureInfo.InvariantCulture));
        }
        r.AppendLine();
      }
    }

    public string GetReport() {
      if (threads == null)
        return null;
      
      StringBuilder r = new StringBuilder();
      
      r.AppendLine("CPUID");
      r.AppendLine();

      for (int i = 0; i < threads.Length; i++) {

        r.AppendLine("Processor " + i);
        r.AppendLine();
        r.AppendFormat("Processor Vendor: {0}{1}", threads[i][0][0].Vendor,
          Environment.NewLine);
        r.AppendFormat("Processor Brand: {0}{1}", threads[i][0][0].BrandString,
          Environment.NewLine);
        r.AppendFormat("Family: 0x{0}{1}", 
          threads[i][0][0].Family.ToString("X", CultureInfo.InvariantCulture), 
          Environment.NewLine);
        r.AppendFormat("Model: 0x{0}{1}",
          threads[i][0][0].Model.ToString("X", CultureInfo.InvariantCulture), 
          Environment.NewLine);
        r.AppendFormat("Stepping: 0x{0}{1}",
          threads[i][0][0].Stepping.ToString("X", CultureInfo.InvariantCulture), 
          Environment.NewLine);
        r.AppendLine();

        r.AppendLine("CPUID Return Values");
        r.AppendLine();
        for (int j = 0; j < threads[i].Length; j++)
          for (int k = 0; k < threads[i][j].Length; k++) {
            r.AppendLine(" CPU Group: " + threads[i][j][k].Group);
            r.AppendLine(" CPU Thread: " + threads[i][j][k].Thread);
            r.AppendLine(" APIC ID: " + threads[i][j][k].ApicId);
            r.AppendLine(" Processor ID: " + threads[i][j][k].ProcessorId);
            r.AppendLine(" Core ID: " + threads[i][j][k].CoreId);
            r.AppendLine(" Thread ID: " + threads[i][j][k].ThreadId);
            r.AppendLine();
            r.AppendLine(" Function  EAX       EBX       ECX       EDX");
            AppendCpuidData(r, threads[i][j][k].Data, CPUID.CPUID_0);
            AppendCpuidData(r, threads[i][j][k].ExtData, CPUID.CPUID_EXT);
            r.AppendLine();
          }
      }
      return r.ToString(); 
    }

    public void Close() {
      foreach (GenericCPU cpu in hardware) {
        cpu.Close();
      }
    }
  }
}
