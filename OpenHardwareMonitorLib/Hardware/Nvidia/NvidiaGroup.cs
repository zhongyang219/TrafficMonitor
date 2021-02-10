/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System.Collections.Generic;
using System.Globalization;
using System.Text;

namespace OpenHardwareMonitor.Hardware.Nvidia {

  internal class NvidiaGroup : IGroup {
   
    private readonly List<Hardware> hardware = new List<Hardware>();
    private readonly StringBuilder report = new StringBuilder();

    public NvidiaGroup(ISettings settings) {
      if (!NVAPI.IsAvailable)
        return;

      report.AppendLine("NVAPI");
      report.AppendLine();

      string version;
      if (NVAPI.NvAPI_GetInterfaceVersionString(out version) == NvStatus.OK) {
        report.Append(" Version: ");
        report.AppendLine(version);
      }

      NvPhysicalGpuHandle[] handles = 
        new NvPhysicalGpuHandle[NVAPI.MAX_PHYSICAL_GPUS];
      int count;
      if (NVAPI.NvAPI_EnumPhysicalGPUs == null) {
        report.AppendLine(" Error: NvAPI_EnumPhysicalGPUs not available");
        report.AppendLine();
        return;
      } else {        
        NvStatus status = NVAPI.NvAPI_EnumPhysicalGPUs(handles, out count);
        if (status != NvStatus.OK) {
          report.AppendLine(" Status: " + status);
          report.AppendLine();
          return;
        }
      }

      var result = NVML.NvmlInit();

      report.AppendLine();
      report.AppendLine("NVML");
      report.AppendLine();
      report.AppendLine(" Status: " + result);
      report.AppendLine();

      IDictionary<NvPhysicalGpuHandle, NvDisplayHandle> displayHandles =
        new Dictionary<NvPhysicalGpuHandle, NvDisplayHandle>();

      if (NVAPI.NvAPI_EnumNvidiaDisplayHandle != null &&
        NVAPI.NvAPI_GetPhysicalGPUsFromDisplay != null) 
      {
        NvStatus status = NvStatus.OK;
        int i = 0;
        while (status == NvStatus.OK) {
          NvDisplayHandle displayHandle = new NvDisplayHandle();
          status = NVAPI.NvAPI_EnumNvidiaDisplayHandle(i, ref displayHandle);
          i++;

          if (status == NvStatus.OK) {
            NvPhysicalGpuHandle[] handlesFromDisplay =
              new NvPhysicalGpuHandle[NVAPI.MAX_PHYSICAL_GPUS];
            uint countFromDisplay;
            if (NVAPI.NvAPI_GetPhysicalGPUsFromDisplay(displayHandle,
              handlesFromDisplay, out countFromDisplay) == NvStatus.OK) {
              for (int j = 0; j < countFromDisplay; j++) {
                if (!displayHandles.ContainsKey(handlesFromDisplay[j]))
                  displayHandles.Add(handlesFromDisplay[j], displayHandle);
              }
            }
          }
        }
      }

      report.Append("Number of GPUs: ");
      report.AppendLine(count.ToString(CultureInfo.InvariantCulture));

      for (int i = 0; i < count; i++) {
        NvDisplayHandle displayHandle;
        displayHandles.TryGetValue(handles[i], out displayHandle);
        hardware.Add(new NvidiaGPU(i, handles[i], displayHandle, settings));
      }

      report.AppendLine();
    }

    public IHardware[] Hardware {
      get {
        return hardware.ToArray();
      }
    }

    public string GetReport() {
      return report.ToString();
    }

    public void Close() {
      foreach (Hardware gpu in hardware)
        gpu.Close();

      if (NVML.IsInitialized) {
        NVML.NvmlShutdown();
      }
    }
  }
}
