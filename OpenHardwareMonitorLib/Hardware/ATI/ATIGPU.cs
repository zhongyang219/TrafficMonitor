/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Globalization;
using System.Text;

namespace OpenHardwareMonitor.Hardware.ATI {
  internal sealed class ATIGPU : Hardware {

    private readonly int adapterIndex;
    private readonly int busNumber;
    private readonly int deviceNumber;
    private readonly Sensor temperatureCore;
    private readonly Sensor temperatureMemory;
    private readonly Sensor temperatureVrmCore;
    private readonly Sensor temperatureVrmMemory;
    private readonly Sensor temperatureVrmMemory0;
    private readonly Sensor temperatureVrmMemory1;
    private readonly Sensor temperatureLiquid;
    private readonly Sensor temperaturePlx;
    private readonly Sensor temperatureHotSpot;
    private readonly Sensor temperatureVrmSoc;
    private readonly Sensor powerCore;
    private readonly Sensor powerPpt;
    private readonly Sensor powerSocket;
    private readonly Sensor powerTotal;
    private readonly Sensor powerSoc;
    private readonly Sensor fan;
    private readonly Sensor coreClock;
    private readonly Sensor memoryClock;
    private readonly Sensor socClock;
    private readonly Sensor coreVoltage;
    private readonly Sensor memoryVoltage;
    private readonly Sensor socVoltage;
    private readonly Sensor coreLoad;
    private readonly Sensor memoryLoad;
    private readonly Sensor controlSensor;
    private readonly Control fanControl;

    private IntPtr context;
    private readonly int overdriveVersion;

    public ATIGPU(string name, int adapterIndex, int busNumber, 
      int deviceNumber, IntPtr context, ISettings settings) 
      : base(name, new Identifier("atigpu", 
        adapterIndex.ToString(CultureInfo.InvariantCulture)), settings)
    {
      this.adapterIndex = adapterIndex;
      this.busNumber = busNumber;
      this.deviceNumber = deviceNumber;

      this.context = context;

      if (ADL.ADL_Overdrive_Caps(adapterIndex, out _, out _,
        out overdriveVersion) != ADLStatus.OK)
      {
        overdriveVersion = -1;
      }

      this.temperatureCore = 
        new Sensor("GPU Core", 0, SensorType.Temperature, this, settings);
      this.temperatureMemory = 
        new Sensor("GPU Memory", 1, SensorType.Temperature, this, settings);
      this.temperatureVrmCore = 
        new Sensor("GPU VRM Core", 2, SensorType.Temperature, this, settings);
      this.temperatureVrmMemory = 
        new Sensor("GPU VRM Memory", 3, SensorType.Temperature, this, settings);
      this.temperatureVrmMemory0 =
        new Sensor("GPU VRM Memory #1", 4, SensorType.Temperature, this, settings);
      this.temperatureVrmMemory1 =
        new Sensor("GPU VRM Memory #2", 5, SensorType.Temperature, this, settings);
      this.temperatureVrmSoc =
        new Sensor("GPU VRM SOC", 6, SensorType.Temperature, this, settings);
      this.temperatureLiquid =
        new Sensor("GPU Liquid", 7, SensorType.Temperature, this, settings);
      this.temperaturePlx = 
        new Sensor("GPU PLX", 8, SensorType.Temperature, this, settings);
      this.temperatureHotSpot = 
        new Sensor("GPU Hot Spot", 9, SensorType.Temperature, this, settings);

      this.powerTotal = new Sensor("GPU Total", 0, SensorType.Power, this, settings);
      this.powerCore = new Sensor("GPU Core", 1, SensorType.Power, this, settings);
      this.powerPpt = new Sensor("GPU PPT", 2, SensorType.Power, this, settings);
      this.powerSocket = new Sensor("GPU Socket", 3, SensorType.Power, this, settings);
      this.powerSoc = new Sensor("GPU SOC", 4, SensorType.Power, this, settings);

      this.fan = new Sensor("GPU Fan", 0, SensorType.Fan, this, settings);

      this.coreClock = new Sensor("GPU Core", 0, SensorType.Clock, this, settings);
      this.memoryClock = new Sensor("GPU Memory", 1, SensorType.Clock, this, settings);
      this.socClock = new Sensor("GPU SOC", 2, SensorType.Clock, this, settings);

      this.coreVoltage = new Sensor("GPU Core", 0, SensorType.Voltage, this, settings);
      this.memoryVoltage = new Sensor("GPU Memory", 1, SensorType.Voltage, this, settings);
      this.socVoltage = new Sensor("GPU SOC", 2, SensorType.Voltage, this, settings);

      this.coreLoad = new Sensor("GPU Core", 0, SensorType.Load, this, settings);
      this.memoryLoad = new Sensor("GPU Memory", 1, SensorType.Load, this, settings);

      this.controlSensor = new Sensor("GPU Fan", 0, SensorType.Control, this, settings);

      ADLFanSpeedInfo afsi = new ADLFanSpeedInfo();
      if (ADL.ADL_Overdrive5_FanSpeedInfo_Get(adapterIndex, 0, ref afsi)
        != ADLStatus.OK) 
      {
        afsi.MaxPercent = 100;
        afsi.MinPercent = 0;
      }

      this.fanControl = new Control(controlSensor, settings, afsi.MinPercent, 
        afsi.MaxPercent);
      this.fanControl.ControlModeChanged += ControlModeChanged;
      this.fanControl.SoftwareControlValueChanged += 
        SoftwareControlValueChanged;
      ControlModeChanged(fanControl);
      this.controlSensor.Control = fanControl;
      Update();                   
    }

    private void SoftwareControlValueChanged(IControl control) {
      if (control.ControlMode == ControlMode.Software) {        
        ADLFanSpeedValue adlf = new ADLFanSpeedValue();
        adlf.SpeedType = ADL.ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;
        adlf.Flags = ADL.ADL_DL_FANCTRL_FLAG_USER_DEFINED_SPEED;
        adlf.FanSpeed = (int)control.SoftwareValue;
        ADL.ADL_Overdrive5_FanSpeed_Set(adapterIndex, 0, ref adlf);
      }
    }

    private void ControlModeChanged(IControl control) {
      switch (control.ControlMode) {
        case ControlMode.Undefined:
          return;
        case ControlMode.Default:
          SetDefaultFanSpeed();
          break;
        case ControlMode.Software:
          SoftwareControlValueChanged(control);
          break;
        default:
          return;
      }
    }

    private void SetDefaultFanSpeed() {
      ADL.ADL_Overdrive5_FanSpeedToDefault_Set(adapterIndex, 0);
    }

    public int BusNumber { get { return busNumber; } }

    public int DeviceNumber { get { return deviceNumber; } }


    public override HardwareType HardwareType {
      get { return HardwareType.GpuAti; }
    }

    private void GetODNTemperature(ADLODNTemperatureType type, 
      Sensor sensor) 
    {
      if (ADL.ADL2_OverdriveN_Temperature_Get(context, adapterIndex,
        type, out int temperature) == ADLStatus.OK) 
      {
        sensor.Value = 0.001f * temperature;
        ActivateSensor(sensor);
      } else {
        sensor.Value = null;
      }
    }

    private void GetOD6Power(ADLODNCurrentPowerType type, Sensor sensor) 
    {
      if (ADL.ADL2_Overdrive6_CurrentPower_Get(context, adapterIndex, type, 
        out int power) == ADLStatus.OK) 
      {
        sensor.Value = power * (1.0f / 0xFF);
        ActivateSensor(sensor);
      } else {
        sensor.Value = null;
      }

    }

    public override string GetReport() {
      var r = new StringBuilder();

      r.AppendLine("AMD GPU");
      r.AppendLine();

      r.Append("AdapterIndex: ");
      r.AppendLine(adapterIndex.ToString(CultureInfo.InvariantCulture));
      r.AppendLine();

      r.AppendLine("Overdrive Caps");
      r.AppendLine();
      try {
        var status = ADL.ADL_Overdrive_Caps(adapterIndex,
          out int supported, out int enabled, out int version);
        r.Append(" Status: ");
        r.AppendLine(status.ToString());
        r.Append(" Supported: ");
        r.AppendLine(supported.ToString(CultureInfo.InvariantCulture));
        r.Append(" Enabled: ");
        r.AppendLine(enabled.ToString(CultureInfo.InvariantCulture));
        r.Append(" Version: ");
        r.AppendLine(version.ToString(CultureInfo.InvariantCulture));        
      } catch (Exception e) {
        r.AppendLine(" Status: " + e.Message);
      }
      r.AppendLine();

      r.AppendLine("Overdrive5 Parameters");
      r.AppendLine();
      try {
        var status = ADL.ADL_Overdrive5_ODParameters_Get(
          adapterIndex, out var p);
        r.Append(" Status: ");
        r.AppendLine(status.ToString());
        r.AppendFormat(" NumberOfPerformanceLevels: {0}{1}",
          p.NumberOfPerformanceLevels, Environment.NewLine);
        r.AppendFormat(" ActivityReportingSupported: {0}{1}",
          p.ActivityReportingSupported, Environment.NewLine);
        r.AppendFormat(" DiscretePerformanceLevels: {0}{1}",
          p.DiscretePerformanceLevels, Environment.NewLine);
        r.AppendFormat(" EngineClock.Min: {0}{1}",
          p.EngineClock.Min, Environment.NewLine);
        r.AppendFormat(" EngineClock.Max: {0}{1}",
          p.EngineClock.Max, Environment.NewLine);
        r.AppendFormat(" EngineClock.Step: {0}{1}",
          p.EngineClock.Step, Environment.NewLine);
        r.AppendFormat(" MemoryClock.Min: {0}{1}",
          p.MemoryClock.Min, Environment.NewLine);
        r.AppendFormat(" MemoryClock.Max: {0}{1}",
          p.MemoryClock.Max, Environment.NewLine);
        r.AppendFormat(" MemoryClock.Step: {0}{1}",
          p.MemoryClock.Step, Environment.NewLine);
        r.AppendFormat(" Vddc.Min: {0}{1}",
          p.Vddc.Min, Environment.NewLine);
        r.AppendFormat(" Vddc.Max: {0}{1}",
          p.Vddc.Max, Environment.NewLine);
        r.AppendFormat(" Vddc.Step: {0}{1}",
          p.Vddc.Step, Environment.NewLine);
      } catch (Exception e) {
        r.AppendLine(" Status: " + e.Message);
      }
      r.AppendLine();

      r.AppendLine("Overdrive5 Temperature");
      r.AppendLine();
      try {
        var adlt = new ADLTemperature();
        var status = ADL.ADL_Overdrive5_Temperature_Get(adapterIndex, 0, 
          ref adlt);
        r.Append(" Status: ");
        r.AppendLine(status.ToString());
        r.AppendFormat(" Value: {0}{1}", 
          0.001f * adlt.Temperature, Environment.NewLine);
      } catch (Exception e) {
        r.AppendLine(" Status: " + e.Message);
      }
      r.AppendLine();

      r.AppendLine("Overdrive5 FanSpeed");
      r.AppendLine();
      try {
        var adlf = new ADLFanSpeedValue();
        adlf.SpeedType = ADL.ADL_DL_FANCTRL_SPEED_TYPE_RPM;
        var status = ADL.ADL_Overdrive5_FanSpeed_Get(adapterIndex, 0, ref adlf);
        r.Append(" Status RPM: ");
        r.AppendLine(status.ToString());
        r.AppendFormat(" Value RPM: {0}{1}",
          adlf.FanSpeed, Environment.NewLine);
        adlf.SpeedType = ADL.ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;
        status = ADL.ADL_Overdrive5_FanSpeed_Get(adapterIndex, 0, ref adlf);
        r.Append(" Status Percent: ");
        r.AppendLine(status.ToString());
        r.AppendFormat(" Value Percent: {0}{1}",
          adlf.FanSpeed, Environment.NewLine);
      } catch (Exception e) {
        r.AppendLine(" Status: " + e.Message);
      }
      r.AppendLine();

      r.AppendLine("Overdrive5 CurrentActivity");
      r.AppendLine();
      try {
        var adlp = new ADLPMActivity();
        var status = ADL.ADL_Overdrive5_CurrentActivity_Get(adapterIndex,
          ref adlp);
        r.Append(" Status: ");
        r.AppendLine(status.ToString());
        r.AppendFormat(" EngineClock: {0}{1}",
          0.01f * adlp.EngineClock, Environment.NewLine);
        r.AppendFormat(" MemoryClock: {0}{1}",
          0.01f * adlp.MemoryClock, Environment.NewLine);
        r.AppendFormat(" Vddc: {0}{1}",
          0.001f * adlp.Vddc, Environment.NewLine);
        r.AppendFormat(" ActivityPercent: {0}{1}",
          adlp.ActivityPercent, Environment.NewLine);
        r.AppendFormat(" CurrentPerformanceLevel: {0}{1}",
          adlp.CurrentPerformanceLevel, Environment.NewLine);
        r.AppendFormat(" CurrentBusSpeed: {0}{1}",
          adlp.CurrentBusSpeed, Environment.NewLine);
        r.AppendFormat(" CurrentBusLanes: {0}{1}",
          adlp.CurrentBusLanes, Environment.NewLine);
        r.AppendFormat(" MaximumBusLanes: {0}{1}",
          adlp.MaximumBusLanes, Environment.NewLine);
      } catch (Exception e) {
        r.AppendLine(" Status: " + e.Message);
      }
      r.AppendLine();

      if (context != IntPtr.Zero) {
        r.AppendLine("Overdrive6 CurrentPower");
        r.AppendLine();
        try {
          for (int i = 0; i < 4; i++) {
            var pt = ((ADLODNCurrentPowerType)i).ToString();
            var status = ADL.ADL2_Overdrive6_CurrentPower_Get(
              context, adapterIndex, (ADLODNCurrentPowerType)i,
              out int power);
            if (status == ADLStatus.OK) {
              r.AppendFormat(" Power[{0}].Value: {1}{2}", pt,
                power * (1.0f / 0xFF), Environment.NewLine);
            } else {
              r.AppendFormat(" Power[{0}].Status: {1}{2}", pt,
                status.ToString(), Environment.NewLine);
            }
          }          
        } catch (EntryPointNotFoundException) {
          r.AppendLine(" Status: Entry point not found");
        } catch (Exception e) {
          r.AppendLine(" Status: " + e.Message);
        }
        r.AppendLine();
      }

      if (context != IntPtr.Zero) {
        r.AppendLine("OverdriveN Temperature");
        r.AppendLine();
        try {
          for (int i = 1; i < 8; i++) {
            var tt = ((ADLODNTemperatureType)i).ToString();
            var status = ADL.ADL2_OverdriveN_Temperature_Get(
              context, adapterIndex, (ADLODNTemperatureType)i,
              out int temperature);
            if (status == ADLStatus.OK) {
              r.AppendFormat(" Temperature[{0}].Value: {1}{2}", tt,
                0.001f * temperature, Environment.NewLine);
            } else {
              r.AppendFormat(" Temperature[{0}].Status: {1}{2}", tt,
                status.ToString(), Environment.NewLine);
            }
          }
        } catch (EntryPointNotFoundException) {
          r.AppendLine(" Status: Entry point not found");
        } catch (Exception e) {
          r.AppendLine(" Status: " + e.Message);
        }
        r.AppendLine();
      }

      if (context != IntPtr.Zero) {
        r.AppendLine("OverdriveN Performance Status");
        r.AppendLine();
        try {          
          var status = ADL.ADL2_OverdriveN_PerformanceStatus_Get(context, 
            adapterIndex, out var ps);
          r.Append(" Status: ");
          r.AppendLine(status.ToString());
          r.AppendFormat(" CoreClock: {0}{1}", 
            ps.CoreClock, Environment.NewLine);
          r.AppendFormat(" MemoryClock: {0}{1}", 
            ps.MemoryClock, Environment.NewLine);
          r.AppendFormat(" DCEFClock: {0}{1}", 
            ps.DCEFClock, Environment.NewLine);
          r.AppendFormat(" GFXClock: {0}{1}", 
            ps.GFXClock, Environment.NewLine);
          r.AppendFormat(" UVDClock: {0}{1}", 
            ps.UVDClock, Environment.NewLine);
          r.AppendFormat(" VCEClock: {0}{1}", 
            ps.VCEClock, Environment.NewLine);
          r.AppendFormat(" GPUActivityPercent: {0}{1}", 
            ps.GPUActivityPercent, Environment.NewLine);
          r.AppendFormat(" CurrentCorePerformanceLevel: {0}{1}", 
            ps.CurrentCorePerformanceLevel, Environment.NewLine);
          r.AppendFormat(" CurrentMemoryPerformanceLevel: {0}{1}", 
            ps.CurrentMemoryPerformanceLevel, Environment.NewLine);
          r.AppendFormat(" CurrentDCEFPerformanceLevel: {0}{1}", 
            ps.CurrentDCEFPerformanceLevel, Environment.NewLine);
          r.AppendFormat(" CurrentGFXPerformanceLevel: {0}{1}", 
            ps.CurrentGFXPerformanceLevel, Environment.NewLine);
          r.AppendFormat(" UVDPerformanceLevel: {0}{1}", 
            ps.UVDPerformanceLevel, Environment.NewLine);
          r.AppendFormat(" VCEPerformanceLevel: {0}{1}", 
            ps.VCEPerformanceLevel, Environment.NewLine);
          r.AppendFormat(" CurrentBusSpeed: {0}{1}", 
            ps.CurrentBusSpeed, Environment.NewLine);
          r.AppendFormat(" CurrentBusLanes: {0}{1}", 
            ps.CurrentBusLanes, Environment.NewLine);
          r.AppendFormat(" MaximumBusLanes: {0}{1}", 
            ps.MaximumBusLanes, Environment.NewLine);
          r.AppendFormat(" VDDC: {0}{1}", 
            ps.VDDC, Environment.NewLine);
          r.AppendFormat(" VDDCI: {0}{1}", 
            ps.VDDCI, Environment.NewLine);
        } catch (EntryPointNotFoundException) {
          r.AppendLine(" Status: Entry point not found");
        } catch (Exception e) {
          r.AppendLine(" Status: " + e.Message);
        }
        r.AppendLine();
      }

      if (context != IntPtr.Zero) {
        r.AppendLine("Performance Metrics");
        r.AppendLine();
        try {
          var status = ADL.ADL2_New_QueryPMLogData_Get(context, adapterIndex, 
            out var data);
          if (status == ADLStatus.OK) {
            for (int i = 0; i < data.Sensors.Length; i++) {
              if (data.Sensors[i].Supported) {
                var st = ((ADLSensorType)i).ToString();
                r.AppendFormat(" Sensor[{0}].Value: {1}{2}", st,
                  data.Sensors[i].Value, Environment.NewLine);
              }
            }
          } else {
            r.Append(" Status: ");
            r.AppendLine(status.ToString());
          }
        } catch (EntryPointNotFoundException) {
          r.AppendLine(" Status: Entry point not found");
        } catch (Exception e) {
          r.AppendLine(" Status: " + e.Message);
        }

        r.AppendLine();
      }

      return r.ToString();
    }

    private void GetPMLog(ADLPMLogDataOutput data, 
      ADLSensorType sensorType, Sensor sensor, float factor = 1.0f) 
    {
      int i = (int)sensorType;
      if (i < data.Sensors.Length && data.Sensors[i].Supported) {
        sensor.Value = data.Sensors[i].Value * factor;
        ActivateSensor(sensor);
      }
    }

    public override void Update() {
      if (context != IntPtr.Zero && overdriveVersion >= 8 && 
        ADL.ADL2_New_QueryPMLogData_Get(context, adapterIndex, 
        out var data) == ADLStatus.OK) 
      {
        GetPMLog(data, ADLSensorType.TEMPERATURE_EDGE, temperatureCore);
        GetPMLog(data, ADLSensorType.TEMPERATURE_MEM, temperatureMemory);
        GetPMLog(data, ADLSensorType.TEMPERATURE_VRVDDC, temperatureVrmCore);
        GetPMLog(data, ADLSensorType.TEMPERATURE_VRMVDD, temperatureVrmMemory);
        GetPMLog(data, ADLSensorType.TEMPERATURE_VRMVDD0, temperatureVrmMemory0);
        GetPMLog(data, ADLSensorType.TEMPERATURE_VRMVDD1, temperatureVrmMemory1);
        GetPMLog(data, ADLSensorType.TEMPERATURE_VRSOC, temperatureVrmSoc);
        GetPMLog(data, ADLSensorType.TEMPERATURE_LIQUID, temperatureLiquid);
        GetPMLog(data, ADLSensorType.TEMPERATURE_PLX, temperaturePlx);
        GetPMLog(data, ADLSensorType.TEMPERATURE_HOTSPOT, temperatureHotSpot);
        GetPMLog(data, ADLSensorType.GFX_POWER, powerCore);
        GetPMLog(data, ADLSensorType.ASIC_POWER, powerTotal);
        GetPMLog(data, ADLSensorType.SOC_POWER, powerSoc);
        GetPMLog(data, ADLSensorType.FAN_RPM, fan);
        GetPMLog(data, ADLSensorType.CLK_GFXCLK, coreClock);
        GetPMLog(data, ADLSensorType.CLK_MEMCLK, memoryClock);
        GetPMLog(data, ADLSensorType.CLK_SOCCLK, socClock);
        GetPMLog(data, ADLSensorType.GFX_VOLTAGE, coreVoltage, 0.001f);
        GetPMLog(data, ADLSensorType.MEM_VOLTAGE, memoryVoltage, 0.001f);
        GetPMLog(data, ADLSensorType.SOC_VOLTAGE, socVoltage, 0.001f);
        GetPMLog(data, ADLSensorType.INFO_ACTIVITY_GFX, coreLoad);
        GetPMLog(data, ADLSensorType.INFO_ACTIVITY_MEM, memoryLoad);
        GetPMLog(data, ADLSensorType.FAN_PERCENTAGE, controlSensor);
      } else {
        if (context != IntPtr.Zero && overdriveVersion >= 7) {
          GetODNTemperature(ADLODNTemperatureType.CORE, temperatureCore);
          GetODNTemperature(ADLODNTemperatureType.MEMORY, temperatureMemory);
          GetODNTemperature(ADLODNTemperatureType.VRM_CORE, temperatureVrmCore);
          GetODNTemperature(ADLODNTemperatureType.VRM_MEMORY, temperatureVrmMemory);
          GetODNTemperature(ADLODNTemperatureType.LIQUID, temperatureLiquid);
          GetODNTemperature(ADLODNTemperatureType.PLX, temperaturePlx);
          GetODNTemperature(ADLODNTemperatureType.HOTSPOT, temperatureHotSpot);
        } else {
          ADLTemperature adlt = new ADLTemperature();
          if (ADL.ADL_Overdrive5_Temperature_Get(adapterIndex, 0, ref adlt)
            == ADLStatus.OK) 
          {
            temperatureCore.Value = 0.001f * adlt.Temperature;
            ActivateSensor(temperatureCore);
          } else {
            temperatureCore.Value = null;
          }
        }

        if (context != IntPtr.Zero && overdriveVersion >= 6) {
          GetOD6Power(ADLODNCurrentPowerType.TOTAL_POWER, powerTotal);
          GetOD6Power(ADLODNCurrentPowerType.CHIP_POWER, powerCore);
          GetOD6Power(ADLODNCurrentPowerType.PPT_POWER, powerPpt);
          GetOD6Power(ADLODNCurrentPowerType.SOCKET_POWER, powerSocket);
        }

        ADLFanSpeedValue adlf = new ADLFanSpeedValue();
        adlf.SpeedType = ADL.ADL_DL_FANCTRL_SPEED_TYPE_RPM;
        if (ADL.ADL_Overdrive5_FanSpeed_Get(adapterIndex, 0, ref adlf)
          == ADLStatus.OK) 
        {
          fan.Value = adlf.FanSpeed;
          ActivateSensor(fan);
        } else {
          fan.Value = null;
        }

        adlf = new ADLFanSpeedValue();
        adlf.SpeedType = ADL.ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;
        if (ADL.ADL_Overdrive5_FanSpeed_Get(adapterIndex, 0, ref adlf)
          == ADLStatus.OK) 
        {
          controlSensor.Value = adlf.FanSpeed;
          ActivateSensor(controlSensor);
        } else {
          controlSensor.Value = null;
        }

        ADLPMActivity adlp = new ADLPMActivity();
        if (ADL.ADL_Overdrive5_CurrentActivity_Get(adapterIndex, ref adlp)
          == ADLStatus.OK) 
        {
          if (adlp.EngineClock > 0) {
            coreClock.Value = 0.01f * adlp.EngineClock;
            ActivateSensor(coreClock);
          } else {
            coreClock.Value = null;
          }

          if (adlp.MemoryClock > 0) {
            memoryClock.Value = 0.01f * adlp.MemoryClock;
            ActivateSensor(memoryClock);
          } else {
            memoryClock.Value = null;
          }

          if (adlp.Vddc > 0) {
            coreVoltage.Value = 0.001f * adlp.Vddc;
            ActivateSensor(coreVoltage);
          } else {
            coreVoltage.Value = null;
          }

          if (adlp.ActivityPercent >= 0 && adlp.ActivityPercent <= 100) {
            coreLoad.Value = adlp.ActivityPercent;
            ActivateSensor(coreLoad);
          } else {
            coreLoad.Value = null;
          }
        } else {
          coreClock.Value = null;
          memoryClock.Value = null;
          coreVoltage.Value = null;
          coreLoad.Value = null;
        }
      }
    }

    public override void Close() {
      this.fanControl.ControlModeChanged -= ControlModeChanged;
      this.fanControl.SoftwareControlValueChanged -=
        SoftwareControlValueChanged;

      if (this.fanControl.ControlMode != ControlMode.Undefined)
        SetDefaultFanSpeed();
      base.Close();
    }
  }
}
