/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2011 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text;

namespace OpenHardwareMonitor.Hardware.TBalancer {
  internal class TBalancer : Hardware {

    private readonly int portIndex;    
    private readonly byte protocolVersion;
    private readonly Sensor[] digitalTemperatures = new Sensor[8];
    private readonly Sensor[] analogTemperatures = new Sensor[4];
    private readonly Sensor[] sensorhubTemperatures = new Sensor[6];
    private readonly Sensor[] sensorhubFlows = new Sensor[2];
    private readonly Sensor[] fans = new Sensor[4];
    private readonly Sensor[] controls = new Sensor[4];
    private readonly Sensor[] miniNGTemperatures = new Sensor[4];
    private readonly Sensor[] miniNGFans = new Sensor[4];
    private readonly Sensor[] miniNGControls = new Sensor[4];
    private readonly List<ISensor> deactivating = new List<ISensor>();

    private FT_HANDLE handle;
    private byte[] data = new byte[285];
    private byte[] primaryData = new byte[0];
    private byte[] alternativeData = new byte[0];

    public const byte STARTFLAG = 100;
    public const byte ENDFLAG = 254;

    private delegate void MethodDelegate();
    private readonly MethodDelegate alternativeRequest;

    public TBalancer(int portIndex, byte protocolVersion, ISettings settings)
      : base("T-Balancer bigNG",  new Identifier("bigng",
        portIndex.ToString(CultureInfo.InvariantCulture)), settings) 
    {

      this.portIndex = portIndex;
      this.protocolVersion = protocolVersion;

      ParameterDescription[] parameter = new [] {
        new ParameterDescription("Offset [°C]", "Temperature offset.", 0)
      };
      int offset = 0;
      for (int i = 0; i < digitalTemperatures.Length; i++)
        digitalTemperatures[i] = new Sensor("Digital Sensor " + i,
          offset + i, SensorType.Temperature, this, parameter, settings);
      offset += digitalTemperatures.Length;

      for (int i = 0; i < analogTemperatures.Length; i++)
        analogTemperatures[i] = new Sensor("Analog Sensor " + (i + 1),
          offset + i, SensorType.Temperature, this, parameter, settings);
      offset += analogTemperatures.Length;

      for (int i = 0; i < sensorhubTemperatures.Length; i++)
        sensorhubTemperatures[i] = new Sensor("Sensorhub Sensor " + i,
          offset + i, SensorType.Temperature, this, parameter, settings);
      offset += sensorhubTemperatures.Length;

      for (int i = 0; i < miniNGTemperatures.Length; i++)
        miniNGTemperatures[i] = new Sensor("miniNG #" + (i / 2 + 1) +
          " Sensor " + (i % 2 + 1), offset + i, SensorType.Temperature,
          this, parameter, settings);
      offset += miniNGTemperatures.Length;

      for (int i = 0; i < sensorhubFlows.Length; i++)
        sensorhubFlows[i] = new Sensor("Flowmeter " + (i + 1),
          i, SensorType.Flow, this, new [] {
            new ParameterDescription("Impulse Rate", 
              "The impulse rate of the flowmeter in pulses/L", 509)
          }, settings);

      for (int i = 0; i < controls.Length; i++) {
        controls[i] = new Sensor("Fan Channel " + i, i, SensorType.Control, 
          this, settings);
      }

      for (int i = 0; i < miniNGControls.Length; i++) {
        miniNGControls[i] = new Sensor("miniNG #" + (i / 2 + 1) +
          " Fan Channel " + (i % 2 + 1), 4 + i, SensorType.Control, this, 
          settings);
      }

      alternativeRequest = new MethodDelegate(DelayedAlternativeRequest);

      Open();
      Update(); 
    }

    protected override void ActivateSensor(ISensor sensor) {
      deactivating.Remove(sensor);
      base.ActivateSensor(sensor);   
    } 

    protected override void DeactivateSensor(ISensor sensor) {
      if (deactivating.Contains(sensor)) {
        deactivating.Remove(sensor);
        base.DeactivateSensor(sensor);
      } else if (active.Contains(sensor)) {
        deactivating.Add(sensor);
      }     
    }

    private void ReadminiNG(int number) {
      int offset = 1 + number * 65;

      if (data[offset + 61] != ENDFLAG)
        return;

      for (int i = 0; i < 2; i++) {
        Sensor sensor = miniNGTemperatures[number * 2 + i];
        if (data[offset + 7 + i] > 0) {
          sensor.Value = 0.5f * data[offset + 7 + i] + 
            sensor.Parameters[0].Value;
          ActivateSensor(sensor);
        } else {
          DeactivateSensor(sensor);
        }
      }

      for (int i = 0; i < 2; i++) {
        if (miniNGFans[number * 2 + i] == null)
          miniNGFans[number * 2 + i] = 
            new Sensor("miniNG #" + (number + 1) + " Fan Channel " + (i + 1),
            4 + number * 2 + i, SensorType.Fan, this, settings);
        
        Sensor sensor = miniNGFans[number * 2 + i];

        sensor.Value = 20.0f * data[offset + 43 + 2 * i];
        ActivateSensor(sensor);
      }

      for (int i = 0; i < 2; i++) {
        Sensor sensor = miniNGControls[number * 2 + i];
        sensor.Value = data[offset + 15 + i];
        ActivateSensor(sensor);
      }
    }

    private void ReadData() {
      FTD2XX.Read(handle, data);
      
      if (data[0] != STARTFLAG) {
        FTD2XX.FT_Purge(handle, FT_PURGE.FT_PURGE_RX);   
        return;
      }

      if (data[1] == 255 || data[1] == 88) { // bigNG

        if (data[274] != protocolVersion) 
          return;

        if (primaryData.Length == 0)
          primaryData = new byte[data.Length];
        data.CopyTo(primaryData, 0);

        for (int i = 0; i < digitalTemperatures.Length; i++)
          if (data[238 + i] > 0) {
            digitalTemperatures[i].Value = 0.5f * data[238 + i] + 
              digitalTemperatures[i].Parameters[0].Value;
            ActivateSensor(digitalTemperatures[i]);
          } else {
            DeactivateSensor(digitalTemperatures[i]);
          }

        for (int i = 0; i < analogTemperatures.Length; i++)
          if (data[260 + i] > 0) {
            analogTemperatures[i].Value = 0.5f * data[260 + i] +
              analogTemperatures[i].Parameters[0].Value;
            ActivateSensor(analogTemperatures[i]);
          } else {
            DeactivateSensor(analogTemperatures[i]);
          }

        for (int i = 0; i < sensorhubTemperatures.Length; i++)
          if (data[246 + i] > 0) {
            sensorhubTemperatures[i].Value = 0.5f * data[246 + i] +
              sensorhubTemperatures[i].Parameters[0].Value;
            ActivateSensor(sensorhubTemperatures[i]);
          } else {
            DeactivateSensor(sensorhubTemperatures[i]);
          }

        for (int i = 0; i < sensorhubFlows.Length; i++)
          if (data[231 + i] > 0 && data[234] > 0) {
            float pulsesPerSecond = (data[231 + i] * 4.0f) / data[234];
            float pulsesPerLiter = sensorhubFlows[i].Parameters[0].Value;
            sensorhubFlows[i].Value = pulsesPerSecond * 3600 / pulsesPerLiter;
            ActivateSensor(sensorhubFlows[i]);
          } else {
            DeactivateSensor(sensorhubFlows[i]);
          }
        
        for (int i = 0; i < fans.Length; i++) {
          float maxRPM = 11.5f * ((data[149 + 2 * i] << 8) | data[148 + 2 * i]);

          if (fans[i] == null)
            fans[i] = new Sensor("Fan Channel " + i, i, SensorType.Fan,
              this, new [] { new ParameterDescription("MaxRPM", 
                  "Maximum revolutions per minute (RPM) of the fan.", maxRPM)
              }, settings);

          float value;
          if ((data[136] & (1 << i)) == 0)  // pwm mode
            value = 0.02f * data[137 + i];
          else // analog mode
            value = 0.01f * data[141 + i];
          
          fans[i].Value = fans[i].Parameters[0].Value * value;
          ActivateSensor(fans[i]);

          controls[i].Value = 100 * value;
          ActivateSensor(controls[i]);
        }

      } else if (data[1] == 253) { // miniNG #1
        if (alternativeData.Length == 0)
          alternativeData = new byte[data.Length];
        data.CopyTo(alternativeData, 0);

        ReadminiNG(0);        
              
        if (data[66] == 253)  // miniNG #2
          ReadminiNG(1);
      } 
    }

    public override HardwareType HardwareType {
      get { return HardwareType.TBalancer; }
    }

    public override string GetReport() {
      StringBuilder r = new StringBuilder();

      r.AppendLine("T-Balancer bigNG");
      r.AppendLine();
      r.Append("Port Index: "); 
      r.AppendLine(portIndex.ToString(CultureInfo.InvariantCulture));
      r.AppendLine();

      r.AppendLine("Primary System Information Answer");
      r.AppendLine();
      r.AppendLine("       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
      r.AppendLine();
      for (int i = 0; i <= 0x11; i++) {
        r.Append(" "); 
        r.Append((i << 4).ToString("X3", CultureInfo.InvariantCulture)); 
        r.Append("  ");
        for (int j = 0; j <= 0xF; j++) {
          int index = ((i << 4) | j);
          if (index < primaryData.Length) {
            r.Append(" ");
            r.Append(primaryData[index].ToString("X2", CultureInfo.InvariantCulture));
          }          
        }
        r.AppendLine();
      }
      r.AppendLine();

      if (alternativeData.Length > 0) {
        r.AppendLine("Alternative System Information Answer");
        r.AppendLine();
        r.AppendLine("       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
        r.AppendLine();
        for (int i = 0; i <= 0x11; i++) {
          r.Append(" "); 
          r.Append((i << 4).ToString("X3", CultureInfo.InvariantCulture)); 
          r.Append("  ");
          for (int j = 0; j <= 0xF; j++) {
            int index = ((i << 4) | j);
            if (index < alternativeData.Length) {
              r.Append(" ");
              r.Append(alternativeData[index].ToString("X2", CultureInfo.InvariantCulture));
            }
          }
          r.AppendLine();
        }
        r.AppendLine();
      }

      return r.ToString();
    }

    private void DelayedAlternativeRequest() {
      System.Threading.Thread.Sleep(500);      
      FTD2XX.Write(handle, new byte[] { 0x37 });
    }

    public void Open() {
      FTD2XX.FT_Open(portIndex, out handle); 
      FTD2XX.FT_SetBaudRate(handle, 19200);
      FTD2XX.FT_SetDataCharacteristics(handle, 8, 1, 0);
      FTD2XX.FT_SetFlowControl(handle, FT_FLOW_CONTROL.FT_FLOW_RTS_CTS, 0x11,
        0x13);
      FTD2XX.FT_SetTimeouts(handle, 1000, 1000);
      FTD2XX.FT_Purge(handle, FT_PURGE.FT_PURGE_ALL);
    }

    public override void Update() {
      while (FTD2XX.BytesToRead(handle) >= 285)
        ReadData();
      if (FTD2XX.BytesToRead(handle) == 1)
        FTD2XX.ReadByte(handle);

      FTD2XX.Write(handle, new byte[] { 0x38 });
      alternativeRequest.BeginInvoke(null, null);
    }

    public override void Close() {
      FTD2XX.FT_Close(handle);
      base.Close();
    }

  }
}
