/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2012 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text;
using System.Threading;

namespace OpenHardwareMonitor.Hardware.TBalancer {
  internal class TBalancerGroup : IGroup {

    private readonly List<TBalancer> hardware = new List<TBalancer>();
    private readonly StringBuilder report = new StringBuilder();

    public TBalancerGroup(ISettings settings) {

      uint numDevices;
      try {
        if (FTD2XX.FT_CreateDeviceInfoList(out numDevices) != FT_STATUS.FT_OK) {
          report.AppendLine("Status: FT_CreateDeviceInfoList failed");
          return;
        }
      } catch (DllNotFoundException) { return; } 
        catch (ArgumentNullException) { return; }
        catch (EntryPointNotFoundException) { return; }
        catch (BadImageFormatException) { return; }
   
      FT_DEVICE_INFO_NODE[] info = new FT_DEVICE_INFO_NODE[numDevices];
      if (FTD2XX.FT_GetDeviceInfoList(info, ref numDevices) != FT_STATUS.FT_OK) 
      {
        report.AppendLine("Status: FT_GetDeviceInfoList failed");        
        return;
      }
 
      // make sure numDevices is not larger than the info array
      if (numDevices > info.Length)
        numDevices = (uint)info.Length;

      for (int i = 0; i < numDevices; i++) {
        report.Append("Device Index: ");
        report.AppendLine(i.ToString(CultureInfo.InvariantCulture));
        report.Append("Device Type: ");
        report.AppendLine(info[i].Type.ToString());

        // the T-Balancer always uses an FT232BM
        if (info[i].Type != FT_DEVICE.FT_DEVICE_232BM) {
          report.AppendLine("Status: Wrong device type");
          continue;
        }

        FT_HANDLE handle;
        FT_STATUS status = FTD2XX.FT_Open(i, out handle);
        if (status != FT_STATUS.FT_OK) {
          report.AppendLine("Open Status: " + status);
          continue;
        }

        FTD2XX.FT_SetBaudRate(handle, 19200);
        FTD2XX.FT_SetDataCharacteristics(handle, 8, 1, 0);
        FTD2XX.FT_SetFlowControl(handle, FT_FLOW_CONTROL.FT_FLOW_RTS_CTS, 0x11, 
          0x13);
        FTD2XX.FT_SetTimeouts(handle, 1000, 1000);
        FTD2XX.FT_Purge(handle, FT_PURGE.FT_PURGE_ALL);
        
        status = FTD2XX.Write(handle, new byte[] { 0x38 });
        if (status != FT_STATUS.FT_OK) {
          report.AppendLine("Write Status: " + status);
          FTD2XX.FT_Close(handle);
          continue;
        }

        bool isValid = false;
        byte protocolVersion = 0;

        int j = 0;
        while (FTD2XX.BytesToRead(handle) == 0 && j < 2) {
          Thread.Sleep(100);
          j++;
        }
        if (FTD2XX.BytesToRead(handle) > 0) {
          if (FTD2XX.ReadByte(handle) == TBalancer.STARTFLAG) {
            while (FTD2XX.BytesToRead(handle) < 284 && j < 5) {
              Thread.Sleep(100);
              j++;
            }
            int length = FTD2XX.BytesToRead(handle);
            if (length >= 284) {
              byte[] data = new byte[285];
              data[0] = TBalancer.STARTFLAG;
              for (int k = 1; k < data.Length; k++)
                data[k] = FTD2XX.ReadByte(handle);

              // check protocol version 2X (protocols seen: 2C, 2A, 28)
              isValid = (data[274] & 0xF0) == 0x20;
              protocolVersion = data[274];
              if (!isValid) {
                report.Append("Status: Wrong Protocol Version: 0x");
                report.AppendLine(
                  protocolVersion.ToString("X", CultureInfo.InvariantCulture));
              }
            } else {
              report.AppendLine("Status: Wrong Message Length: " + length);
            }
          } else {
            report.AppendLine("Status: Wrong Startflag");
          }
        } else {
          report.AppendLine("Status: No Response");
        }

        FTD2XX.FT_Purge(handle, FT_PURGE.FT_PURGE_ALL);
        FTD2XX.FT_Close(handle);

        if (isValid) {
          report.AppendLine("Status: OK");
          hardware.Add(new TBalancer(i, protocolVersion, settings));          
        }

        if (i < numDevices - 1)
          report.AppendLine();
      }
    }

    public IHardware[] Hardware {
      get {
        return hardware.ToArray();
      }
    }

    public string GetReport() {
      if (report.Length > 0) {
        StringBuilder r = new StringBuilder();
        r.AppendLine("FTD2XX");
        r.AppendLine();
        r.Append(report);
        r.AppendLine();
        return r.ToString();
      } else
        return null;
    }

    public void Close() {
      foreach (TBalancer tbalancer in hardware)
        tbalancer.Close();
    }
  }
}
