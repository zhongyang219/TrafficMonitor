/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Globalization;
using System.Text;

namespace OpenHardwareMonitor.Hardware.CPU {
  internal sealed class IntelCPU : GenericCPU {

    private enum Microarchitecture {
      Unknown,
      NetBurst,
      Core,
      Atom,
      Nehalem,
      SandyBridge,
      IvyBridge,
      Haswell,
      Broadwell,
      Silvermont,
      Skylake,
      Airmont,
      KabyLake,
      Goldmont,
      GoldmontPlus,
      CannonLake,
      IceLake,
      CometLake,
      Tremont,
      TigerLake
    }

    private readonly Sensor[] coreTemperatures;
    private readonly Sensor packageTemperature;
    private readonly Sensor[] coreClocks;
    private readonly Sensor busClock;
    private readonly Sensor[] powerSensors;

    private readonly Microarchitecture microarchitecture;
    private readonly double timeStampCounterMultiplier;

    private const uint IA32_THERM_STATUS_MSR = 0x019C;
    private const uint IA32_TEMPERATURE_TARGET = 0x01A2;
    private const uint IA32_PERF_STATUS = 0x0198;
    private const uint MSR_PLATFORM_INFO = 0xCE;
    private const uint IA32_PACKAGE_THERM_STATUS = 0x1B1;
    private const uint MSR_RAPL_POWER_UNIT = 0x606;
    private const uint MSR_PKG_ENERY_STATUS = 0x611;
    private const uint MSR_DRAM_ENERGY_STATUS = 0x619;
    private const uint MSR_PP0_ENERY_STATUS = 0x639;
    private const uint MSR_PP1_ENERY_STATUS = 0x641;

    private readonly uint[] energyStatusMSRs = { MSR_PKG_ENERY_STATUS, 
      MSR_PP0_ENERY_STATUS, MSR_PP1_ENERY_STATUS, MSR_DRAM_ENERGY_STATUS };
    private readonly string[] powerSensorLabels = 
      { "CPU Package", "CPU Cores", "CPU Graphics", "CPU DRAM" };
    private float energyUnitMultiplier = 0;
    private DateTime[] lastEnergyTime;
    private uint[] lastEnergyConsumed;


    private float[] Floats(float f) {
      float[] result = new float[coreCount];
      for (int i = 0; i < coreCount; i++)
        result[i] = f;
      return result;
    }

    private float[] GetTjMaxFromMSR() {
      uint eax, edx;
      float[] result = new float[coreCount];
      for (int i = 0; i < coreCount; i++) {
        if (Ring0.RdmsrTx(IA32_TEMPERATURE_TARGET, out eax,
          out edx, cpuid[i][0].Affinity)) {
          result[i] = (eax >> 16) & 0xFF;
        } else {
          result[i] = 100;
        }
      }
      return result;
    }

    public IntelCPU(int processorIndex, CPUID[][] cpuid, ISettings settings)
      : base(processorIndex, cpuid, settings) {
      // set tjMax
      float[] tjMax;
      switch (family) {
        case 0x06: {
            switch (model) {
              case 0x0F: // Intel Core 2 (65nm)
                microarchitecture = Microarchitecture.Core;
                switch (stepping) {
                  case 0x06: // B2
                    switch (coreCount) {
                      case 2:
                        tjMax = Floats(80 + 10); break;
                      case 4:
                        tjMax = Floats(90 + 10); break;
                      default:
                        tjMax = Floats(85 + 10); break;
                    }
                    tjMax = Floats(80 + 10); break;
                  case 0x0B: // G0
                    tjMax = Floats(90 + 10); break;
                  case 0x0D: // M0
                    tjMax = Floats(85 + 10); break;
                  default:
                    tjMax = Floats(85 + 10); break;
                } break;
              case 0x17: // Intel Core 2 (45nm)
                microarchitecture = Microarchitecture.Core;
                tjMax = Floats(100); break;
              case 0x1C: // Intel Atom (45nm)
                microarchitecture = Microarchitecture.Atom;
                switch (stepping) {
                  case 0x02: // C0
                    tjMax = Floats(90); break;
                  case 0x0A: // A0, B0
                    tjMax = Floats(100); break;
                  default:
                    tjMax = Floats(90); break;
                } break;
              case 0x1A: // Intel Core i7 LGA1366 (45nm)
              case 0x1E: // Intel Core i5, i7 LGA1156 (45nm)
              case 0x1F: // Intel Core i5, i7 
              case 0x25: // Intel Core i3, i5, i7 LGA1156 (32nm)
              case 0x2C: // Intel Core i7 LGA1366 (32nm) 6 Core
              case 0x2E: // Intel Xeon Processor 7500 series (45nm)
              case 0x2F: // Intel Xeon Processor (32nm)
                microarchitecture = Microarchitecture.Nehalem;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x2A: // Intel Core i5, i7 2xxx LGA1155 (32nm)
              case 0x2D: // Next Generation Intel Xeon, i7 3xxx LGA2011 (32nm)
                microarchitecture = Microarchitecture.SandyBridge;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x3A: // Intel Core i5, i7 3xxx LGA1155 (22nm)
              case 0x3E: // Intel Core i7 4xxx LGA2011 (22nm)
                microarchitecture = Microarchitecture.IvyBridge;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x3C: // Intel Core i5, i7 4xxx LGA1150 (22nm)              
              case 0x3F: // Intel Xeon E5-2600/1600 v3, Core i7-59xx
                         // LGA2011-v3, Haswell-E (22nm)
              case 0x45: // Intel Core i5, i7 4xxxU (22nm)
              case 0x46: 
                microarchitecture = Microarchitecture.Haswell;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x3D: // Intel Core M-5xxx (14nm)
              case 0x47: // Intel i5, i7 5xxx, Xeon E3-1200 v4 (14nm)
              case 0x4F: // Intel Xeon E5-26xx v4
              case 0x56: // Intel Xeon D-15xx
                microarchitecture = Microarchitecture.Broadwell;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x36: // Intel Atom S1xxx, D2xxx, N2xxx (32nm)
                microarchitecture = Microarchitecture.Atom;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x37: // Intel Atom E3xxx, Z3xxx (22nm)
              case 0x4A:
              case 0x4D: // Intel Atom C2xxx (22nm)
              case 0x5A:
              case 0x5D:
                microarchitecture = Microarchitecture.Silvermont;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x4E:
              case 0x5E: // Intel Core i5, i7 6xxxx LGA1151 (14nm)
              case 0x55: // Intel Core i7, i9 7xxxx LGA2066 (14nm)
                microarchitecture = Microarchitecture.Skylake;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x4C:
                microarchitecture = Microarchitecture.Airmont;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x8E: 
              case 0x9E: // Intel Core i5, i7 7xxxx (14nm)
                microarchitecture = Microarchitecture.KabyLake;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x5C: // Intel Atom processors (Apollo Lake) (14nm)
              case 0x5F: // Intel Atom processors (Denverton) (14nm)
                microarchitecture = Microarchitecture.Goldmont;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x7A: // Intel Atom processors (14nm)
                microarchitecture = Microarchitecture.GoldmontPlus;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x66: // Intel Core i3 8121U (10nm)
                microarchitecture = Microarchitecture.CannonLake;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x7D: // Intel Core i3, i5, i7 10xxGx (10nm) 
              case 0x7E: 
              case 0x6A: // Intel Xeon (10nm)
              case 0x6C:
                microarchitecture = Microarchitecture.IceLake;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0xA5:
              case 0xA6: // Intel Core i3, i5, i7 10xxxU (14nm)
                microarchitecture = Microarchitecture.CometLake;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x86: // Intel Atom processors
                microarchitecture = Microarchitecture.Tremont;
                tjMax = GetTjMaxFromMSR();
                break;
              case 0x8C: // Intel processors (10nm++)
              case 0x8D:
                microarchitecture = Microarchitecture.TigerLake;
                tjMax = GetTjMaxFromMSR();
                break;
              default:
                microarchitecture = Microarchitecture.Unknown;
                tjMax = Floats(100);
                break;
            }
          } break;
        case 0x0F: {
            switch (model) {
              case 0x00: // Pentium 4 (180nm)
              case 0x01: // Pentium 4 (130nm)
              case 0x02: // Pentium 4 (130nm)
              case 0x03: // Pentium 4, Celeron D (90nm)
              case 0x04: // Pentium 4, Pentium D, Celeron D (90nm)
              case 0x06: // Pentium 4, Pentium D, Celeron D (65nm)
                microarchitecture = Microarchitecture.NetBurst;
                tjMax = Floats(100);
                break;
              default:
                microarchitecture = Microarchitecture.Unknown;
                tjMax = Floats(100);
                break;
            }
          } break;
        default:
          microarchitecture = Microarchitecture.Unknown;
          tjMax = Floats(100);
          break;
      }

      // set timeStampCounterMultiplier
      switch (microarchitecture) {
        case Microarchitecture.NetBurst:
        case Microarchitecture.Atom:
        case Microarchitecture.Core: {
            uint eax, edx;
            if (Ring0.Rdmsr(IA32_PERF_STATUS, out eax, out edx)) {
              timeStampCounterMultiplier =
                ((edx >> 8) & 0x1f) + 0.5 * ((edx >> 14) & 1);
            }
          } break;
        case Microarchitecture.Nehalem:
        case Microarchitecture.SandyBridge:
        case Microarchitecture.IvyBridge:
        case Microarchitecture.Haswell: 
        case Microarchitecture.Broadwell:
        case Microarchitecture.Silvermont:
        case Microarchitecture.Skylake:
        case Microarchitecture.Airmont:
        case Microarchitecture.KabyLake:
        case Microarchitecture.Goldmont:
        case Microarchitecture.GoldmontPlus:
        case Microarchitecture.CannonLake:
        case Microarchitecture.IceLake:
        case Microarchitecture.CometLake:
        case Microarchitecture.Tremont:
        case Microarchitecture.TigerLake: {
            uint eax, edx;
            if (Ring0.Rdmsr(MSR_PLATFORM_INFO, out eax, out edx)) {
              timeStampCounterMultiplier = (eax >> 8) & 0xff;
            }
          } break;
        default: 
          timeStampCounterMultiplier = 0;
          break;
      }

      // check if processor supports a digital thermal sensor at core level
      if (cpuid[0][0].Data.GetLength(0) > 6 &&
        (cpuid[0][0].Data[6, 0] & 1) != 0 && 
        microarchitecture != Microarchitecture.Unknown) 
      {
        coreTemperatures = new Sensor[coreCount];
        for (int i = 0; i < coreTemperatures.Length; i++) {
          coreTemperatures[i] = new Sensor(CoreString(i), i,
            SensorType.Temperature, this, new[] { 
              new ParameterDescription(
                "TjMax [°C]", "TjMax temperature of the core sensor.\n" + 
                "Temperature = TjMax - TSlope * Value.", tjMax[i]), 
              new ParameterDescription("TSlope [°C]", 
                "Temperature slope of the digital thermal sensor.\n" + 
                "Temperature = TjMax - TSlope * Value.", 1)}, settings);
          ActivateSensor(coreTemperatures[i]);
        }
      } else {
        coreTemperatures = new Sensor[0];
      }

      // check if processor supports a digital thermal sensor at package level
      if (cpuid[0][0].Data.GetLength(0) > 6 &&
        (cpuid[0][0].Data[6, 0] & 0x40) != 0 && 
        microarchitecture != Microarchitecture.Unknown) 
      {
        packageTemperature = new Sensor("CPU Package",
          coreTemperatures.Length, SensorType.Temperature, this, new[] { 
              new ParameterDescription(
                "TjMax [°C]", "TjMax temperature of the package sensor.\n" + 
                "Temperature = TjMax - TSlope * Value.", tjMax[0]), 
              new ParameterDescription("TSlope [°C]", 
                "Temperature slope of the digital thermal sensor.\n" + 
                "Temperature = TjMax - TSlope * Value.", 1)}, settings);
        ActivateSensor(packageTemperature);
      }

      busClock = new Sensor("Bus Speed", 0, SensorType.Clock, this, settings);
      coreClocks = new Sensor[coreCount];
      for (int i = 0; i < coreClocks.Length; i++) {
        coreClocks[i] =
          new Sensor(CoreString(i), i + 1, SensorType.Clock, this, settings);
        if (HasTimeStampCounter && microarchitecture != Microarchitecture.Unknown)
          ActivateSensor(coreClocks[i]);
      }

      if (microarchitecture == Microarchitecture.SandyBridge ||
          microarchitecture == Microarchitecture.IvyBridge ||
          microarchitecture == Microarchitecture.Haswell ||
          microarchitecture == Microarchitecture.Broadwell || 
          microarchitecture == Microarchitecture.Skylake ||
          microarchitecture == Microarchitecture.Silvermont ||
          microarchitecture == Microarchitecture.Airmont ||
          microarchitecture == Microarchitecture.KabyLake || 
          microarchitecture == Microarchitecture.Goldmont ||
          microarchitecture == Microarchitecture.GoldmontPlus ||
          microarchitecture == Microarchitecture.CannonLake ||
          microarchitecture == Microarchitecture.IceLake ||
          microarchitecture == Microarchitecture.CometLake ||
          microarchitecture == Microarchitecture.Tremont ||
          microarchitecture == Microarchitecture.TigerLake) 
      {
        powerSensors = new Sensor[energyStatusMSRs.Length];
        lastEnergyTime = new DateTime[energyStatusMSRs.Length];
        lastEnergyConsumed = new uint[energyStatusMSRs.Length];

        uint eax, edx;
        if (Ring0.Rdmsr(MSR_RAPL_POWER_UNIT, out eax, out edx))
          switch (microarchitecture) {
            case Microarchitecture.Silvermont:
            case Microarchitecture.Airmont:
              energyUnitMultiplier = 1.0e-6f * (1 << (int)((eax >> 8) & 0x1F));
              break;
            default:
              energyUnitMultiplier = 1.0f / (1 << (int)((eax >> 8) & 0x1F));
              break;
          }
        if (energyUnitMultiplier != 0) {
          for (int i = 0; i < energyStatusMSRs.Length; i++) {
            if (!Ring0.Rdmsr(energyStatusMSRs[i], out eax, out edx))
              continue;

            lastEnergyTime[i] = DateTime.UtcNow;
            lastEnergyConsumed[i] = eax;
            powerSensors[i] = new Sensor(powerSensorLabels[i], i,
              SensorType.Power, this, settings);
            ActivateSensor(powerSensors[i]);
          }
        }
      }

      Update();
    }

    protected override uint[] GetMSRs() {
      return new[] {
        MSR_PLATFORM_INFO,
        IA32_PERF_STATUS ,
        IA32_THERM_STATUS_MSR,
        IA32_TEMPERATURE_TARGET,
        IA32_PACKAGE_THERM_STATUS,
        MSR_RAPL_POWER_UNIT,
        MSR_PKG_ENERY_STATUS,
        MSR_DRAM_ENERGY_STATUS,
        MSR_PP0_ENERY_STATUS,
        MSR_PP1_ENERY_STATUS
      };
    }

    public override string GetReport() {
      StringBuilder r = new StringBuilder();
      r.Append(base.GetReport());

      r.Append("Microarchitecture: ");
      r.AppendLine(microarchitecture.ToString());
      r.Append("Time Stamp Counter Multiplier: ");
      r.AppendLine(timeStampCounterMultiplier.ToString(
        CultureInfo.InvariantCulture));
      r.AppendLine();

      return r.ToString();
    }

    public override void Update() {
      base.Update();

      for (int i = 0; i < coreTemperatures.Length; i++) {
        uint eax, edx;
        // if reading is valid
        if (Ring0.RdmsrTx(IA32_THERM_STATUS_MSR, out eax, out edx,
            cpuid[i][0].Affinity) && (eax & 0x80000000) != 0) 
        {
          // get the dist from tjMax from bits 22:16
          float deltaT = ((eax & 0x007F0000) >> 16);
          float tjMax = coreTemperatures[i].Parameters[0].Value;
          float tSlope = coreTemperatures[i].Parameters[1].Value;
          coreTemperatures[i].Value = tjMax - tSlope * deltaT;
        } else {
          coreTemperatures[i].Value = null;
        }
      }

      if (packageTemperature != null) {
        uint eax, edx;
        // if reading is valid
        if (Ring0.RdmsrTx(IA32_PACKAGE_THERM_STATUS, out eax, out edx,
            cpuid[0][0].Affinity) && (eax & 0x80000000) != 0) 
        {
          // get the dist from tjMax from bits 22:16
          float deltaT = ((eax & 0x007F0000) >> 16);
          float tjMax = packageTemperature.Parameters[0].Value;
          float tSlope = packageTemperature.Parameters[1].Value;
          packageTemperature.Value = tjMax - tSlope * deltaT;
        } else {
          packageTemperature.Value = null;
        }
      }

      if (HasTimeStampCounter && timeStampCounterMultiplier > 0) {
        double newBusClock = 0;
        uint eax, edx;
        for (int i = 0; i < coreClocks.Length; i++) {
          System.Threading.Thread.Sleep(1);
          if (Ring0.RdmsrTx(IA32_PERF_STATUS, out eax, out edx, 
            cpuid[i][0].Affinity)) 
          {
            newBusClock =
              TimeStampCounterFrequency / timeStampCounterMultiplier;
            switch (microarchitecture) {
              case Microarchitecture.Nehalem: {
                  uint multiplier = eax & 0xff;
                  coreClocks[i].Value = (float)(multiplier * newBusClock);
                } break;
              case Microarchitecture.SandyBridge:
              case Microarchitecture.IvyBridge:
              case Microarchitecture.Haswell: 
              case Microarchitecture.Broadwell:
              case Microarchitecture.Silvermont:
              case Microarchitecture.Skylake:
              case Microarchitecture.KabyLake: 
              case Microarchitecture.Goldmont:
              case Microarchitecture.GoldmontPlus:
              case Microarchitecture.CannonLake:
              case Microarchitecture.IceLake:
              case Microarchitecture.CometLake:
              case Microarchitecture.Tremont:
              case Microarchitecture.TigerLake: {
                  uint multiplier = (eax >> 8) & 0xff;
                  coreClocks[i].Value = (float)(multiplier * newBusClock);
                } break;
              default: {
                  double multiplier =
                    ((eax >> 8) & 0x1f) + 0.5 * ((eax >> 14) & 1);
                  coreClocks[i].Value = (float)(multiplier * newBusClock);
                } break;
            }
          } else {
            // if IA32_PERF_STATUS is not available, assume TSC frequency
            coreClocks[i].Value = (float)TimeStampCounterFrequency;
          }
        }
        if (newBusClock > 0) {
          this.busClock.Value = (float)newBusClock;
          ActivateSensor(this.busClock);
        }
      }

      if (powerSensors != null) {
        foreach (Sensor sensor in powerSensors) {
          if (sensor == null)
            continue;

          uint eax, edx;
          if (!Ring0.Rdmsr(energyStatusMSRs[sensor.Index], out eax, out edx))
            continue;

          DateTime time = DateTime.UtcNow;
          uint energyConsumed = eax;
          float deltaTime =
            (float)(time - lastEnergyTime[sensor.Index]).TotalSeconds;
          if (deltaTime < 0.01)
            continue;

          sensor.Value = energyUnitMultiplier * unchecked(
            energyConsumed - lastEnergyConsumed[sensor.Index]) / deltaTime;
          lastEnergyTime[sensor.Index] = time;
          lastEnergyConsumed[sensor.Index] = energyConsumed;
        }
      }
    }
  }
}
