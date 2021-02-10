/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Collections.Generic;
using System.Threading;
using OpenHardwareMonitor.Hardware.LPC;

namespace OpenHardwareMonitor.Hardware.Mainboard {
  internal sealed class SuperIOHardware : Hardware {

    private readonly Mainboard mainboard;
    private readonly ISuperIO superIO;

    private readonly List<Sensor> voltages = new List<Sensor>();
    private readonly List<Sensor> temperatures = new List<Sensor>();
    private readonly List<Sensor> fans = new List<Sensor>();
    private readonly List<Sensor> controls = new List<Sensor>();

    private delegate float? ReadValueDelegate(int index);
    private delegate void UpdateDelegate();

    // delegates for mainboard specific sensor reading code
    private readonly ReadValueDelegate readVoltage;
    private readonly ReadValueDelegate readTemperature;
    private readonly ReadValueDelegate readFan;
    private readonly ReadValueDelegate readControl;

    // delegate for post update mainboard specific code
    private readonly UpdateDelegate postUpdate;

    // mainboard specific mutex
    private readonly Mutex mutex;

    public SuperIOHardware(Mainboard mainboard, ISuperIO superIO, 
      Manufacturer manufacturer, Model model, ISettings settings) 
      : base(ChipName.GetName(superIO.Chip), new Identifier("lpc", 
        superIO.Chip.ToString().ToLowerInvariant()), settings)
    {
      this.mainboard = mainboard;
      this.superIO = superIO;

      IList<Voltage> v;
      IList<Temperature> t;
      IList<Fan> f;
      IList<Ctrl> c;
      GetBoardSpecificConfiguration(superIO, manufacturer, model,
        out v, out t, out f, out c,
        out readVoltage, out readTemperature, out readFan, out readControl,
        out postUpdate, out mutex);

      CreateVoltageSensors(superIO, settings, v);
      CreateTemperatureSensors(superIO, settings, t);
      CreateFanSensors(superIO, settings, f);
      CreateControlSensors(superIO, settings, c);
    }

    private void CreateControlSensors(ISuperIO superIO, ISettings settings, 
      IList<Ctrl> c) 
    {
      foreach (Ctrl ctrl in c) {
        int index = ctrl.Index;
        if (index < superIO.Controls.Length) {
          Sensor sensor = new Sensor(ctrl.Name, index, SensorType.Control,
            this, settings);
          Control control = new Control(sensor, settings, 0, 100);
          control.ControlModeChanged += (cc) => {
            switch (cc.ControlMode) {
              case ControlMode.Undefined:
                return;
              case ControlMode.Default:
                superIO.SetControl(index, null);
                break;
              case ControlMode.Software:
                superIO.SetControl(index, (byte)(cc.SoftwareValue * 2.55));
                break;
              default:
                return;
            }
          };
          control.SoftwareControlValueChanged += (cc) => {
            if (cc.ControlMode == ControlMode.Software)
              superIO.SetControl(index, (byte)(cc.SoftwareValue * 2.55));
          };

          switch (control.ControlMode) {
            case ControlMode.Undefined:
              break;
            case ControlMode.Default:
              superIO.SetControl(index, null);
              break;
            case ControlMode.Software:
              superIO.SetControl(index, (byte)(control.SoftwareValue * 2.55));
              break;
            default:
              break;
          }            

          sensor.Control = control;
          controls.Add(sensor);
          ActivateSensor(sensor);
        }
      }
    }

    private void CreateFanSensors(ISuperIO superIO, ISettings settings, 
      IList<Fan> f) 
    {
      foreach (Fan fan in f) {
        if (fan.Index < superIO.Fans.Length) {
          Sensor sensor = new Sensor(fan.Name, fan.Index, SensorType.Fan,
            this, settings);
          fans.Add(sensor);
        }
      }
    }

    private void CreateTemperatureSensors(ISuperIO superIO, ISettings settings, 
      IList<Temperature> t) 
    {
      foreach (Temperature temperature in t)
        if (temperature.Index < superIO.Temperatures.Length) {
          Sensor sensor = new Sensor(temperature.Name, temperature.Index,
            SensorType.Temperature, this, new[] {
          new ParameterDescription("Offset [°C]", "Temperature offset.", 0)
        }, settings);
          temperatures.Add(sensor);
        }
    }

    private void CreateVoltageSensors(ISuperIO superIO, ISettings settings, 
      IList<Voltage> v) 
    {
      const string formula = "Voltage = value + (value - Vf) * Ri / Rf.";
      foreach (Voltage voltage in v)
        if (voltage.Index < superIO.Voltages.Length) {
          Sensor sensor = new Sensor(voltage.Name, voltage.Index,
            voltage.Hidden, SensorType.Voltage, this, new[] {
            new ParameterDescription("Ri [kΩ]", "Input resistance.\n" + 
              formula, voltage.Ri),
            new ParameterDescription("Rf [kΩ]", "Reference resistance.\n" + 
              formula, voltage.Rf),
            new ParameterDescription("Vf [V]", "Reference voltage.\n" + 
              formula, voltage.Vf)
            }, settings);
          voltages.Add(sensor);
        }
    }

    private static void GetBoardSpecificConfiguration(ISuperIO superIO,
      Manufacturer manufacturer, Model model, out IList<Voltage> v,
      out IList<Temperature> t, out IList<Fan> f, out IList<Ctrl> c,
      out ReadValueDelegate readVoltage,
      out ReadValueDelegate readTemperature,
      out ReadValueDelegate readFan,
      out ReadValueDelegate readControl,
      out UpdateDelegate postUpdate, out Mutex mutex) {
      readVoltage = (index) => superIO.Voltages[index];
      readTemperature = (index) => superIO.Temperatures[index];
      readFan = (index) => superIO.Fans[index];
      readControl = (index) => superIO.Controls[index];

      postUpdate = () => { };
      mutex = null;

      v = new List<Voltage>();
      t = new List<Temperature>();
      f = new List<Fan>();
      c = new List<Ctrl>();

      switch (superIO.Chip) {
        case Chip.IT8705F:
        case Chip.IT8712F:
        case Chip.IT8716F:
        case Chip.IT8718F:
        case Chip.IT8720F:
        case Chip.IT8726F:
          GetITEConfigurationsA(superIO, manufacturer, model, v, t, f, c,
            ref readFan, ref postUpdate, ref mutex);
          break;

        case Chip.IT8620E:
        case Chip.IT8628E:
        case Chip.IT8655E:
        case Chip.IT8665E:
        case Chip.IT8686E:
        case Chip.IT8688E:
        case Chip.IT8721F:
        case Chip.IT8728F:
        case Chip.IT8771E:
        case Chip.IT8772E:
          GetITEConfigurationsB(superIO, manufacturer, model, v, t, f, c);
          break;

        case Chip.IT879XE:
          GetITEConfigurationsC(superIO, manufacturer, model, v, t, f, c);
          break;

        case Chip.F71858:
          v.Add(new Voltage("VCC3V", 0, 150, 150));
          v.Add(new Voltage("VSB3V", 1, 150, 150));
          v.Add(new Voltage("Battery", 2, 150, 150));
          for (int i = 0; i < superIO.Temperatures.Length; i++)
            t.Add(new Temperature("Temperature #" + (i + 1), i));
          for (int i = 0; i < superIO.Fans.Length; i++)
            f.Add(new Fan("Fan #" + (i + 1), i));
          break;
        case Chip.F71862:
        case Chip.F71869:
        case Chip.F71869A:
        case Chip.F71882:
        case Chip.F71889AD:
        case Chip.F71889ED:
        case Chip.F71889F:
        case Chip.F71808E:
          GetFintekConfiguration(superIO, manufacturer, model, v, t, f, c);
          break;

        case Chip.W83627EHF:
          GetWinbondConfigurationEHF(manufacturer, model, v, t, f);
          break;
        case Chip.W83627DHG:
        case Chip.W83627DHGP:
        case Chip.W83667HG:
        case Chip.W83667HGB:
          GetWinbondConfigurationHG(manufacturer, model, v, t, f);
          break;
        case Chip.W83627HF:
        case Chip.W83627THF:
        case Chip.W83687THF:
          v.Add(new Voltage("CPU VCore", 0));
          v.Add(new Voltage("Voltage #2", 1, true));
          v.Add(new Voltage("Voltage #3", 2, true));
          v.Add(new Voltage("AVCC", 3, 34, 51));
          v.Add(new Voltage("Voltage #5", 4, true));
          v.Add(new Voltage("5VSB", 5, 34, 51));
          v.Add(new Voltage("VBAT", 6));
          t.Add(new Temperature("CPU", 0));
          t.Add(new Temperature("Auxiliary", 1));
          t.Add(new Temperature("System", 2));
          f.Add(new Fan("System Fan", 0));
          f.Add(new Fan("CPU Fan", 1));
          f.Add(new Fan("Auxiliary Fan", 2));
          break;
        case Chip.NCT6771F:
        case Chip.NCT6776F:
          GetNuvotonConfigurationF(superIO, manufacturer, model, v, t, f, c);
          break;
        case Chip.NCT610X:
          v.Add(new Voltage("CPU VCore", 0));
          v.Add(new Voltage("Voltage #0", 1, true));
          v.Add(new Voltage("AVCC", 2, 34, 34));
          v.Add(new Voltage("3VCC", 3, 34, 34));
          v.Add(new Voltage("Voltage #1", 4, true));
          v.Add(new Voltage("Voltage #2", 5, true));
          v.Add(new Voltage("Reserved", 6, true));
          v.Add(new Voltage("3VSB", 7, 34, 34));
          v.Add(new Voltage("VBAT", 8, 34, 34));
          v.Add(new Voltage("Voltage #10", 9, true));
          t.Add(new Temperature("SYS", 1));
          t.Add(new Temperature("CPU Core", 2));
          t.Add(new Temperature("AUX", 3));
          for (int i = 0; i < superIO.Fans.Length; i++)
            f.Add(new Fan("Fan #" + (i + 1), i));
          for (int i = 0; i < superIO.Controls.Length; i++)
            c.Add(new Ctrl("Fan Control #" + (i + 1), i));
          break;
        case Chip.NCT6779D:
        case Chip.NCT6791D:
        case Chip.NCT6792D:
        case Chip.NCT6792DA:
        case Chip.NCT6793D:
        case Chip.NCT6795D:
        case Chip.NCT6796D:
        case Chip.NCT6796DR:
        case Chip.NCT6797D:
        case Chip.NCT6798D:
          GetNuvotonConfigurationD(superIO, manufacturer, model, v, t, f, c);
          break;
        default:
          GetDefaultConfiguration(superIO, v, t, f, c);
          break;
      }
    }

    private static void GetDefaultConfiguration(ISuperIO superIO, 
      IList<Voltage> v, IList<Temperature> t, IList<Fan> f, IList<Ctrl> c) 
    {
      for (int i = 0; i < superIO.Voltages.Length; i++)
        v.Add(new Voltage("Voltage #" + (i + 1), i, true));
      for (int i = 0; i < superIO.Temperatures.Length; i++)
        t.Add(new Temperature("Temperature #" + (i + 1), i));
      for (int i = 0; i < superIO.Fans.Length; i++)
        f.Add(new Fan("Fan #" + (i + 1), i));
      for (int i = 0; i < superIO.Controls.Length; i++)
        c.Add(new Ctrl("Fan Control #" + (i + 1), i));
    }

    private static void GetITEConfigurationsA(ISuperIO superIO, 
      Manufacturer manufacturer, Model model, 
      IList<Voltage> v, IList<Temperature> t, IList<Fan> f, IList<Ctrl> c, 
      ref ReadValueDelegate readFan, ref UpdateDelegate postUpdate, 
      ref Mutex mutex) 
    {
      switch (manufacturer) {
        case Manufacturer.ASUS:
          switch (model) {
            case Model.Crosshair_III_Formula: // IT8720F
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("CPU", 0));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              break;
            case Model.M2N_SLI_DELUXE:
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("+3.3V", 1));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 4, 30, 10));
              v.Add(new Voltage("+5VSB", 7, 6.8f, 10));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("Motherboard", 1));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("Chassis Fan #1", 1));
              f.Add(new Fan("Power Fan", 2));
              break;
            case Model.M4A79XTD_EVO: // IT8720F           
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("Motherboard", 1));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("Chassis Fan #1", 1));
              f.Add(new Fan("Chassis Fan #2", 2));
              break;
            default:
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("Voltage #2", 1, true));
              v.Add(new Voltage("Voltage #3", 2, true));
              v.Add(new Voltage("Voltage #4", 3, true));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("Voltage #8", 7, true));
              v.Add(new Voltage("VBat", 8));
              for (int i = 0; i < superIO.Temperatures.Length; i++)
                t.Add(new Temperature("Temperature #" + (i + 1), i));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              for (int i = 0; i < superIO.Controls.Length; i++)
                c.Add(new Ctrl("Fan Control #" + (i + 1), i));
              break;
          }
          break;

        case Manufacturer.ASRock:
          switch (model) {
            case Model.P55_Deluxe: // IT8720F
              GetASRockConfiguration(superIO, v, t, f, 
                ref readFan, ref postUpdate, ref mutex);
              break;
            default:
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("Voltage #2", 1, true));
              v.Add(new Voltage("Voltage #3", 2, true));
              v.Add(new Voltage("Voltage #4", 3, true));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("Voltage #8", 7, true));
              v.Add(new Voltage("VBat", 8));
              for (int i = 0; i < superIO.Temperatures.Length; i++)
                t.Add(new Temperature("Temperature #" + (i + 1), i));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              break;
          };
          break;

        case Manufacturer.DFI:
          switch (model) {
            case Model.LP_BI_P45_T2RS_Elite: // IT8718F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("FSB VTT", 1));
              v.Add(new Voltage("+3.3V", 2));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 4, 30, 10));
              v.Add(new Voltage("NB Core", 5));
              v.Add(new Voltage("VDIMM", 6));
              v.Add(new Voltage("+5VSB", 7, 6.8f, 10));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("System", 1));
              t.Add(new Temperature("Chipset", 2));
              f.Add(new Fan("Fan #1", 0));
              f.Add(new Fan("Fan #2", 1));
              f.Add(new Fan("Fan #3", 2));
              break;
            case Model.LP_DK_P55_T3eH9: // IT8720F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("VTT", 1));
              v.Add(new Voltage("+3.3V", 2));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 4, 30, 10));
              v.Add(new Voltage("CPU PLL", 5));
              v.Add(new Voltage("DRAM", 6));
              v.Add(new Voltage("+5VSB", 7, 6.8f, 10));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("Chipset", 0));
              t.Add(new Temperature("CPU PWM", 1));
              t.Add(new Temperature("CPU", 2));
              f.Add(new Fan("Fan #1", 0));
              f.Add(new Fan("Fan #2", 1));
              f.Add(new Fan("Fan #3", 2));
              break;
            default:
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("VTT", 1, true));
              v.Add(new Voltage("+3.3V", 2, true));
              v.Add(new Voltage("+5V", 3, 6.8f, 10, 0, true));
              v.Add(new Voltage("+12V", 4, 30, 10, 0, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("DRAM", 6, true));
              v.Add(new Voltage("+5VSB", 7, 6.8f, 10, 0, true));
              v.Add(new Voltage("VBat", 8));
              for (int i = 0; i < superIO.Temperatures.Length; i++)
                t.Add(new Temperature("Temperature #" + (i + 1), i));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              for (int i = 0; i < superIO.Controls.Length; i++)
                c.Add(new Ctrl("Fan Control #" + (i + 1), i));
              break;
          }
          break;

        case Manufacturer.Gigabyte:
          switch (model) {
            case Model._965P_S3: // IT8718F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1));
              v.Add(new Voltage("+3.3V", 2));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 7, 24.3f, 8.2f));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 1));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan", 1));
              break;
            case Model.EP45_DS3R: // IT8718F
            case Model.EP45_UD3R:
            case Model.X38_DS5:
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1));
              v.Add(new Voltage("+3.3V", 2));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 7, 24.3f, 8.2f));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 1));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan #2", 1));
              f.Add(new Fan("Power Fan", 2));
              f.Add(new Fan("System Fan #1", 3));
              break;
            case Model.EX58_EXTREME: // IT8720F                 
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 1));
              t.Add(new Temperature("Northbridge", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan #2", 1));
              f.Add(new Fan("Power Fan", 2));
              f.Add(new Fan("System Fan #1", 3));
              break;
            case Model.P35_DS3: // IT8718F 
            case Model.P35_DS3L: // IT8718F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1));
              v.Add(new Voltage("+3.3V", 2));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 7, 24.3f, 8.2f));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 1));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan #1", 1));
              f.Add(new Fan("System Fan #2", 2));
              f.Add(new Fan("Power Fan", 3));
              break;
            case Model.P55_UD4: // IT8720F
            case Model.P55A_UD3: // IT8720F
            case Model.P55M_UD4: // IT8720F                
            case Model.H55_USB3: // IT8720F
            case Model.EX58_UD3R: // IT8720F 
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1));
              v.Add(new Voltage("+3.3V", 2));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 5, 24.3f, 8.2f));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan #2", 1));
              f.Add(new Fan("Power Fan", 2));
              f.Add(new Fan("System Fan #1", 3));
              break;
            case Model.H55N_USB3: // IT8720F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1));
              v.Add(new Voltage("+3.3V", 2));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 5, 24.3f, 8.2f));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan", 1));
              break;
            case Model.G41M_Combo: // IT8718F
            case Model.G41MT_S2: // IT8718F
            case Model.G41MT_S2P: // IT8718F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1));
              v.Add(new Voltage("+3.3V", 2));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 7, 24.3f, 8.2f));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("CPU", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan", 1));
              break;
            case Model.GA_970A_UD3: // IT8720F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1));
              v.Add(new Voltage("+3.3V", 2));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 4, 24.3f, 8.2f));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 1));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan #1", 1));
              f.Add(new Fan("System Fan #2", 2));
              f.Add(new Fan("Power Fan", 4));
              c.Add(new Ctrl("PWM 1", 0));
              c.Add(new Ctrl("PWM 2", 1));
              c.Add(new Ctrl("PWM 3", 2));
              break;
            case Model.GA_MA770T_UD3: // IT8720F
            case Model.GA_MA770T_UD3P: // IT8720F                
            case Model.GA_MA790X_UD3P: // IT8720F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1));
              v.Add(new Voltage("+3.3V", 2));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 4, 24.3f, 8.2f));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 1));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan #1", 1));
              f.Add(new Fan("System Fan #2", 2));
              f.Add(new Fan("Power Fan", 3));
              break;
            case Model.GA_MA78LM_S2H: // IT8718F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1));
              v.Add(new Voltage("+3.3V", 2));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 4, 24.3f, 8.2f));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 1));
              t.Add(new Temperature("VRM", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan #1", 1));
              f.Add(new Fan("System Fan #2", 2));
              f.Add(new Fan("Power Fan", 3));
              break;
            case Model.GA_MA785GM_US2H: // IT8718F
            case Model.GA_MA785GMT_UD2H: // IT8718F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1));
              v.Add(new Voltage("+3.3V", 2));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 4, 24.3f, 8.2f));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 1));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan", 1));
              f.Add(new Fan("NB Fan", 2));
              break;
            case Model.X58A_UD3R: // IT8720F 
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1));
              v.Add(new Voltage("+3.3V", 2));
              v.Add(new Voltage("+5V", 3, 6.8f, 10));
              v.Add(new Voltage("+12V", 5, 24.3f, 8.2f));
              v.Add(new Voltage("VBat", 8));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 1));
              t.Add(new Temperature("Northbridge", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan #2", 1));
              f.Add(new Fan("Power Fan", 2));
              f.Add(new Fan("System Fan #1", 3));
              break;
            default:
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1, true));
              v.Add(new Voltage("+3.3V", 2, true));
              v.Add(new Voltage("+5V", 3, 6.8f, 10, 0, true));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("Voltage #8", 7, true));
              v.Add(new Voltage("VBat", 8));
              for (int i = 0; i < superIO.Temperatures.Length; i++)
                t.Add(new Temperature("Temperature #" + (i + 1), i));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              for (int i = 0; i < superIO.Controls.Length; i++)
                c.Add(new Ctrl("Fan Control #" + (i + 1), i));
              break;
          }
          break;

        default:
          v.Add(new Voltage("CPU VCore", 0));
          v.Add(new Voltage("Voltage #2", 1, true));
          v.Add(new Voltage("Voltage #3", 2, true));
          v.Add(new Voltage("Voltage #4", 3, true));
          v.Add(new Voltage("Voltage #5", 4, true));
          v.Add(new Voltage("Voltage #6", 5, true));
          v.Add(new Voltage("Voltage #7", 6, true));
          v.Add(new Voltage("Voltage #8", 7, true));
          v.Add(new Voltage("VBat", 8));
          for (int i = 0; i < superIO.Temperatures.Length; i++)
            t.Add(new Temperature("Temperature #" + (i + 1), i));
          for (int i = 0; i < superIO.Fans.Length; i++)
            f.Add(new Fan("Fan #" + (i + 1), i));
          for (int i = 0; i < superIO.Controls.Length; i++)
            c.Add(new Ctrl("Fan Control #" + (i + 1), i));
          break;
      }
    }

    private static void GetASRockConfiguration(ISuperIO superIO, 
      IList<Voltage> v, IList<Temperature> t, IList<Fan> f, 
      ref ReadValueDelegate readFan, ref UpdateDelegate postUpdate, 
      ref Mutex mutex) 
    {
      v.Add(new Voltage("CPU VCore", 0));
      v.Add(new Voltage("+3.3V", 2));
      v.Add(new Voltage("+12V", 4, 30, 10));
      v.Add(new Voltage("+5V", 5, 6.8f, 10));
      v.Add(new Voltage("VBat", 8));
      t.Add(new Temperature("CPU", 0));
      t.Add(new Temperature("Motherboard", 1));
      f.Add(new Fan("CPU Fan", 0));
      f.Add(new Fan("Chassis Fan #1", 1));

      // this mutex is also used by the official ASRock tool
      mutex = new Mutex(false, "ASRockOCMark");

      bool exclusiveAccess = false;
      try {
        exclusiveAccess = mutex.WaitOne(10, false);
      } catch (AbandonedMutexException) { } 
        catch (InvalidOperationException) { }

      // only read additional fans if we get exclusive access
      if (exclusiveAccess) {

        f.Add(new Fan("Chassis Fan #2", 2));
        f.Add(new Fan("Chassis Fan #3", 3));
        f.Add(new Fan("Power Fan", 4));

        readFan = (index) => {
          if (index < 2) {
            return superIO.Fans[index];
          } else {
            // get GPIO 80-87
            byte? gpio = superIO.ReadGPIO(7);
            if (!gpio.HasValue)
              return null;

            // read the last 3 fans based on GPIO 83-85
            int[] masks = { 0x05, 0x03, 0x06 };
            return (((gpio.Value >> 3) & 0x07) ==
              masks[index - 2]) ? superIO.Fans[2] : null;
          }
        };

        int fanIndex = 0;
        postUpdate = () => {
          // get GPIO 80-87
          byte? gpio = superIO.ReadGPIO(7);
          if (!gpio.HasValue)
            return;

          // prepare the GPIO 83-85 for the next update
          int[] masks = { 0x05, 0x03, 0x06 };
          superIO.WriteGPIO(7,
            (byte)((gpio.Value & 0xC7) | (masks[fanIndex] << 3)));
          fanIndex = (fanIndex + 1) % 3;
        };
      }
    }

    private static void GetITEConfigurationsB(ISuperIO superIO,
      Manufacturer manufacturer, Model model,
      IList<Voltage> v, IList<Temperature> t, IList<Fan> f, IList<Ctrl> c) 
    {
      switch (manufacturer) {
        case Manufacturer.ECS:
          switch (model) {
            case Model.A890GXM_A: // IT8721F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("VDIMM", 1));
              v.Add(new Voltage("NB Voltage", 2));
              v.Add(new Voltage("Analog +3.3V", 3, 10, 10));
              // v.Add(new Voltage("VDIMM", 6, true));
              v.Add(new Voltage("Standby +3.3V", 7, 10, 10));
              v.Add(new Voltage("VBat", 8, 10, 10));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("System", 1));
              t.Add(new Temperature("Northbridge", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan", 1));
              f.Add(new Fan("Power Fan", 2));
              break;
            default:
              v.Add(new Voltage("Voltage #1", 0, true));
              v.Add(new Voltage("Voltage #2", 1, true));
              v.Add(new Voltage("Voltage #3", 2, true));
              v.Add(new Voltage("Analog +3.3V", 3, 10, 10, 0, true));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("Standby +3.3V", 7, 10, 10, 0, true));
              v.Add(new Voltage("VBat", 8, 10, 10));
              for (int i = 0; i < superIO.Temperatures.Length; i++)
                t.Add(new Temperature("Temperature #" + (i + 1), i));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              for (int i = 0; i < superIO.Controls.Length; i++)
                c.Add(new Ctrl("Fan Control #" + (i + 1), i));
              break;
          }
          break;
        case Manufacturer.Gigabyte:
          switch (model) {
            case Model.H61M_DS2_REV_1_2: // IT8728F
            case Model.H61M_USB3_B3_REV_2_0: // IT8728F
              v.Add(new Voltage("VTT", 0));
              v.Add(new Voltage("+12V", 2, 30.9f, 10));
              v.Add(new Voltage("CPU VCore", 5));
              v.Add(new Voltage("DRAM", 6));
              v.Add(new Voltage("Standby +3.3V", 7, 10, 10));
              v.Add(new Voltage("VBat", 8, 10, 10));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan", 1));
              break;
            case Model.H67A_UD3H_B3: // IT8728F
            case Model.H67A_USB3_B3: // IT8728F                
              v.Add(new Voltage("VTT", 0));
              v.Add(new Voltage("+5V", 1, 15, 10));
              v.Add(new Voltage("+12V", 2, 30.9f, 10));
              v.Add(new Voltage("CPU VCore", 5));
              v.Add(new Voltage("DRAM", 6));
              v.Add(new Voltage("Standby +3.3V", 7, 10, 10));
              v.Add(new Voltage("VBat", 8, 10, 10));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan #1", 1));
              f.Add(new Fan("Power Fan", 2));
              f.Add(new Fan("System Fan #2", 3));
              break;
            case Model.X570_AORUS_MASTER: // IT8688E
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("+3.3V", 1, 29.4f, 45.3f));
              v.Add(new Voltage("+12V", 2, 10f, 2f));
              v.Add(new Voltage("+5V", 3, 15f, 10f));
              v.Add(new Voltage("CPU VCore SOC", 4));
              v.Add(new Voltage("CPU VDDP", 5));
              v.Add(new Voltage("DRAM CH(A/B)", 6));
              v.Add(new Voltage("Standby +3.3V", 7, 1f, 1f));
              v.Add(new Voltage("VBat", 8, 1f, 1f));
              t.Add(new Temperature("System 1", 0));
              t.Add(new Temperature("EC_TEMP1", 1));
              t.Add(new Temperature("CPU", 2));
              t.Add(new Temperature("PCIEX16", 3));
              t.Add(new Temperature("VRM MOS", 4));
              t.Add(new Temperature("PCH", 5));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System 1 Fan", 1));
              f.Add(new Fan("System 2 Fan", 2));
              f.Add(new Fan("PCH Fan", 3));
              f.Add(new Fan("CPU OPT Fan", 4));
              c.Add(new Ctrl("CPU Fan", 0));
              c.Add(new Ctrl("System 1 Fan", 1));
              c.Add(new Ctrl("System 2 Fan", 2));
              c.Add(new Ctrl("PCH Fan", 3));
              c.Add(new Ctrl("CPU OPT Fan", 4));
              break;
            case Model.Z390_M_GAMING: // IT8688E
            case Model.Z390_AORUS_ULTRA:
            case Model.Z390_UD:
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("+3.3V", 1, 6.49f, 10));
              v.Add(new Voltage("+12V", 2, 5f, 1));
              v.Add(new Voltage("+5V", 3, 1.5f, 1));
              v.Add(new Voltage("CPU VCCGT", 4));
              v.Add(new Voltage("CPU VCCSA", 5));
              v.Add(new Voltage("VDDQ", 6));
              v.Add(new Voltage("DDRVTT", 7));
              v.Add(new Voltage("PCHCore", 8));
              t.Add(new Temperature("System1", 0));
              t.Add(new Temperature("PCH", 1));
              t.Add(new Temperature("CPU", 2));
              t.Add(new Temperature("PCIEX16", 3));
              t.Add(new Temperature("VRM MOS", 4));
              t.Add(new Temperature("System2", 5));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan #1", 1));
              f.Add(new Fan("System Fan #2", 2));
              f.Add(new Fan("System Fan #3", 3));
              break;
            case Model.Z68A_D3H_B3: // IT8728F
              v.Add(new Voltage("VTT", 0));
              v.Add(new Voltage("+3.3V", 1, 6.49f, 10));
              v.Add(new Voltage("+12V", 2, 30.9f, 10));
              v.Add(new Voltage("+5V", 3, 7.15f, 10));
              v.Add(new Voltage("CPU VCore", 5));
              v.Add(new Voltage("DRAM", 6));
              v.Add(new Voltage("Standby +3.3V", 7, 10, 10));
              v.Add(new Voltage("VBat", 8, 10, 10));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan #1", 1));
              f.Add(new Fan("Power Fan", 2));
              f.Add(new Fan("System Fan #2", 3));
              break;
            case Model.P67A_UD3_B3: // IT8728F
            case Model.P67A_UD3R_B3: // IT8728F
            case Model.P67A_UD4_B3: // IT8728F                
            case Model.Z68AP_D3: // IT8728F
            case Model.Z68X_UD3H_B3: // IT8728F               
              v.Add(new Voltage("VTT", 0));
              v.Add(new Voltage("+3.3V", 1, 6.49f, 10));
              v.Add(new Voltage("+12V", 2, 30.9f, 10));
              v.Add(new Voltage("+5V", 3, 7.15f, 10));
              v.Add(new Voltage("CPU VCore", 5));
              v.Add(new Voltage("DRAM", 6));
              v.Add(new Voltage("Standby +3.3V", 7, 10, 10));
              v.Add(new Voltage("VBat", 8, 10, 10));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("System Fan #2", 1));
              f.Add(new Fan("Power Fan", 2));
              f.Add(new Fan("System Fan #1", 3));
              break;
            case Model.Z68X_UD7_B3: // IT8728F
              v.Add(new Voltage("VTT", 0));
              v.Add(new Voltage("+3.3V", 1, 6.49f, 10));
              v.Add(new Voltage("+12V", 2, 30.9f, 10));
              v.Add(new Voltage("+5V", 3, 7.15f, 10));
              v.Add(new Voltage("CPU VCore", 5));
              v.Add(new Voltage("DRAM", 6));
              v.Add(new Voltage("Standby +3.3V", 7, 10, 10));
              v.Add(new Voltage("VBat", 8, 10, 10));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 1));
              t.Add(new Temperature("System 3", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("Power Fan", 1));
              f.Add(new Fan("System Fan #1", 2));
              f.Add(new Fan("System Fan #2", 3));
              f.Add(new Fan("System Fan #3", 4));
              break;
            default:
              v.Add(new Voltage("Voltage #1", 0, true));
              v.Add(new Voltage("Voltage #2", 1, true));
              v.Add(new Voltage("Voltage #3", 2, true));
              v.Add(new Voltage("Voltage #4", 3, true));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("Standby +3.3V", 7, 10, 10, 0, true));
              v.Add(new Voltage("VBat", 8, 10, 10));
              for (int i = 0; i < superIO.Temperatures.Length; i++)
                t.Add(new Temperature("Temperature #" + (i + 1), i));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              for (int i = 0; i < superIO.Controls.Length; i++)
                c.Add(new Ctrl("Fan Control #" + (i + 1), i));
              break;
          }
          break;
        case Manufacturer.Shuttle:
          switch (model) {
            case Model.FH67: // IT8772E 
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("DRAM", 1));
              v.Add(new Voltage("PCH VCCIO", 2));
              v.Add(new Voltage("CPU VCCIO", 3));
              v.Add(new Voltage("Graphic Voltage", 4));
              v.Add(new Voltage("Standby +3.3V", 7, 10, 10));
              v.Add(new Voltage("VBat", 8, 10, 10));
              t.Add(new Temperature("System", 0));
              t.Add(new Temperature("CPU", 1));
              f.Add(new Fan("Fan #1", 0));
              f.Add(new Fan("CPU Fan", 1));
              break;
            default:
              v.Add(new Voltage("Voltage #1", 0, true));
              v.Add(new Voltage("Voltage #2", 1, true));
              v.Add(new Voltage("Voltage #3", 2, true));
              v.Add(new Voltage("Voltage #4", 3, true));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("Standby +3.3V", 7, 10, 10, 0, true));
              v.Add(new Voltage("VBat", 8, 10, 10));
              for (int i = 0; i < superIO.Temperatures.Length; i++)
                t.Add(new Temperature("Temperature #" + (i + 1), i));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              for (int i = 0; i < superIO.Controls.Length; i++)
                c.Add(new Ctrl("Fan Control #" + (i + 1), i));
              break;
          }
          break;
        default:
          v.Add(new Voltage("Voltage #1", 0, true));
          v.Add(new Voltage("Voltage #2", 1, true));
          v.Add(new Voltage("Voltage #3", 2, true));
          v.Add(new Voltage("Voltage #4", 3, true));
          v.Add(new Voltage("Voltage #5", 4, true));
          v.Add(new Voltage("Voltage #6", 5, true));
          v.Add(new Voltage("Voltage #7", 6, true));
          v.Add(new Voltage("Standby +3.3V", 7, 10, 10, 0, true));
          v.Add(new Voltage("VBat", 8, 10, 10));
          for (int i = 0; i < superIO.Temperatures.Length; i++)
            t.Add(new Temperature("Temperature #" + (i + 1), i));
          for (int i = 0; i < superIO.Fans.Length; i++)
            f.Add(new Fan("Fan #" + (i + 1), i));
          for (int i = 0; i < superIO.Controls.Length; i++)
            c.Add(new Ctrl("Fan Control #" + (i + 1), i));
          break;
      }
    }

    private static void GetITEConfigurationsC(ISuperIO superIO,
      Manufacturer manufacturer, Model model,
      IList<Voltage> v, IList<Temperature> t, IList<Fan> f, IList<Ctrl> c) 
    {
      switch (manufacturer) {        
        case Manufacturer.Gigabyte:
          switch (model) {            
            case Model.X570_AORUS_MASTER: // IT879XE
              v.Add(new Voltage("CPU VDD18", 0));
              v.Add(new Voltage("DDRVTT CH(A/B)", 1));
              v.Add(new Voltage("Chipset Core", 2));
              v.Add(new Voltage("Voltage #4", 3, true));
              v.Add(new Voltage("CPU VDD18", 4));
              v.Add(new Voltage("PM_CLDO12", 5));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("Standby +3.3V", 7, 1f, 1f));
              v.Add(new Voltage("VBat", 8, 1f, 1f));
              t.Add(new Temperature("PCIEX8", 0));
              t.Add(new Temperature("EC_TEMP2", 1));
              t.Add(new Temperature("System 2", 2));
              f.Add(new Fan("System 5 Pump", 0));
              f.Add(new Fan("System 6 Pump", 1));
              f.Add(new Fan("System 4 Fan", 2));
              break;           
            default:
              v.Add(new Voltage("Voltage #1", 0, true));
              v.Add(new Voltage("Voltage #2", 1, true));
              v.Add(new Voltage("Voltage #3", 2, true));
              v.Add(new Voltage("Voltage #4", 3, true));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("Standby +3.3V", 7, 10, 10, 0, true));
              v.Add(new Voltage("VBat", 8, 10, 10));
              for (int i = 0; i < superIO.Temperatures.Length; i++)
                t.Add(new Temperature("Temperature #" + (i + 1), i));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              for (int i = 0; i < superIO.Controls.Length; i++)
                c.Add(new Ctrl("Fan Control #" + (i + 1), i));
              break;
          }
          break;        
        default:
          v.Add(new Voltage("Voltage #1", 0, true));
          v.Add(new Voltage("Voltage #2", 1, true));
          v.Add(new Voltage("Voltage #3", 2, true));
          v.Add(new Voltage("Voltage #4", 3, true));
          v.Add(new Voltage("Voltage #5", 4, true));
          v.Add(new Voltage("Voltage #6", 5, true));
          v.Add(new Voltage("Voltage #7", 6, true));
          v.Add(new Voltage("Standby +3.3V", 7, 10, 10, 0, true));
          v.Add(new Voltage("VBat", 8, 10, 10));
          for (int i = 0; i < superIO.Temperatures.Length; i++)
            t.Add(new Temperature("Temperature #" + (i + 1), i));
          for (int i = 0; i < superIO.Fans.Length; i++)
            f.Add(new Fan("Fan #" + (i + 1), i));
          for (int i = 0; i < superIO.Controls.Length; i++)
            c.Add(new Ctrl("Fan Control #" + (i + 1), i));
          break;
      }
    }

    private static void GetFintekConfiguration(ISuperIO superIO,
      Manufacturer manufacturer, Model model,
      IList<Voltage> v, IList<Temperature> t, IList<Fan> f, IList<Ctrl> c) 
    {
      switch (manufacturer) {
        case Manufacturer.EVGA:
          switch (model) {
            case Model.X58_SLI_Classified: // F71882 
              v.Add(new Voltage("VCC3V", 0, 150, 150));
              v.Add(new Voltage("CPU VCore", 1, 47, 100));
              v.Add(new Voltage("DIMM", 2, 47, 100));
              v.Add(new Voltage("CPU VTT", 3, 24, 100));
              v.Add(new Voltage("IOH Vcore", 4, 24, 100));
              v.Add(new Voltage("+5V", 5, 51, 12));
              v.Add(new Voltage("+12V", 6, 56, 6.8f));
              v.Add(new Voltage("3VSB", 7, 150, 150));
              v.Add(new Voltage("VBat", 8, 150, 150));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("VREG", 1));
              t.Add(new Temperature("System", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("Power Fan", 1));
              f.Add(new Fan("Chassis Fan", 2));
              break;
            default:
              v.Add(new Voltage("VCC3V", 0, 150, 150));
              v.Add(new Voltage("CPU VCore", 1));
              v.Add(new Voltage("Voltage #3", 2, true));
              v.Add(new Voltage("Voltage #4", 3, true));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("VSB3V", 7, 150, 150));
              v.Add(new Voltage("VBat", 8, 150, 150));
              for (int i = 0; i < superIO.Temperatures.Length; i++)
                t.Add(new Temperature("Temperature #" + (i + 1), i));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              break;
          }
          break;
        default:
          v.Add(new Voltage("VCC3V", 0, 150, 150));
          v.Add(new Voltage("CPU VCore", 1));
          v.Add(new Voltage("Voltage #3", 2, true));
          v.Add(new Voltage("Voltage #4", 3, true));
          v.Add(new Voltage("Voltage #5", 4, true));
          v.Add(new Voltage("Voltage #6", 5, true));
          if (superIO.Chip != Chip.F71808E)
            v.Add(new Voltage("Voltage #7", 6, true));
          v.Add(new Voltage("VSB3V", 7, 150, 150));
          v.Add(new Voltage("VBat", 8, 150, 150));
          for (int i = 0; i < superIO.Temperatures.Length; i++)
            t.Add(new Temperature("Temperature #" + (i + 1), i));
          for (int i = 0; i < superIO.Fans.Length; i++)
            f.Add(new Fan("Fan #" + (i + 1), i));
          for (int i = 0; i < superIO.Controls.Length; i++)
            c.Add(new Ctrl("Fan Control #" + (i + 1), i));

          break;
      }
    }

    private static void GetNuvotonConfigurationF(ISuperIO superIO, 
      Manufacturer manufacturer, Model model, 
      IList<Voltage> v, IList<Temperature> t, IList<Fan> f, IList<Ctrl> c) 
    {
      switch (manufacturer) {
        case Manufacturer.ASUS:
          switch (model) {
            case Model.P8P67: // NCT6776F
            case Model.P8P67_EVO: // NCT6776F
            case Model.P8P67_PRO: // NCT6776F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("+12V", 1, 11, 1));
              v.Add(new Voltage("Analog +3.3V", 2, 34, 34));
              v.Add(new Voltage("+3.3V", 3, 34, 34));
              v.Add(new Voltage("+5V", 4, 12, 3));
              v.Add(new Voltage("Standby +3.3V", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("Auxiliary", 2));
              t.Add(new Temperature("Motherboard", 3));
              f.Add(new Fan("Chassis Fan #1", 0));
              f.Add(new Fan("CPU Fan", 1));
              f.Add(new Fan("Power Fan", 2));
              f.Add(new Fan("Chassis Fan #2", 3));
              c.Add(new Ctrl("Chassis Fan #2", 0));
              c.Add(new Ctrl("CPU Fan", 1));
              c.Add(new Ctrl("Chassis Fan #1", 2));
              break;
            case Model.P8P67_M_PRO: // NCT6776F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("+12V", 1, 11, 1));
              v.Add(new Voltage("Analog +3.3V", 2, 34, 34));
              v.Add(new Voltage("+3.3V", 3, 34, 34));
              v.Add(new Voltage("+5V", 4, 12, 3));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("Standby +3.3V", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("Motherboard", 3));
              f.Add(new Fan("Chassis Fan #1", 0));
              f.Add(new Fan("CPU Fan", 1));
              f.Add(new Fan("Chassis Fan #2", 2));
              f.Add(new Fan("Power Fan", 3));
              f.Add(new Fan("Auxiliary Fan", 4));
              break;
            case Model.P8Z68_V_PRO: // NCT6776F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("+12V", 1, 11, 1));
              v.Add(new Voltage("Analog +3.3V", 2, 34, 34));
              v.Add(new Voltage("+3.3V", 3, 34, 34));
              v.Add(new Voltage("+5V", 4, 12, 3));
              v.Add(new Voltage("Standby +3.3V", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("Auxiliary", 2));
              t.Add(new Temperature("Motherboard", 3));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              for (int i = 0; i < superIO.Controls.Length; i++)
                c.Add(new Ctrl("Fan #" + (i + 1), i));
              break;
            case Model.P9X79: // NCT6776F
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("+12V", 1, 11, 1));
              v.Add(new Voltage("Analog +3.3V", 2, 34, 34));
              v.Add(new Voltage("+3.3V", 3, 34, 34));
              v.Add(new Voltage("+5V", 4, 12, 3));
              v.Add(new Voltage("Standby +3.3V", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("Motherboard", 3));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              for (int i = 0; i < superIO.Controls.Length; i++)
                c.Add(new Ctrl("Fan Control #" + (i + 1), i));
              break;
            default:
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("Voltage #2", 1, true));
              v.Add(new Voltage("AVCC", 2, 34, 34));
              v.Add(new Voltage("3VCC", 3, 34, 34));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("3VSB", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              t.Add(new Temperature("CPU Core", 0));
              t.Add(new Temperature("Temperature #1", 1));
              t.Add(new Temperature("Temperature #2", 2));
              t.Add(new Temperature("Temperature #3", 3));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              for (int i = 0; i < superIO.Controls.Length; i++)
                c.Add(new Ctrl("Fan Control #" + (i + 1), i));
              break;
          }
          break;
        default:
          v.Add(new Voltage("CPU VCore", 0));
          v.Add(new Voltage("Voltage #2", 1, true));
          v.Add(new Voltage("AVCC", 2, 34, 34));
          v.Add(new Voltage("3VCC", 3, 34, 34));
          v.Add(new Voltage("Voltage #5", 4, true));
          v.Add(new Voltage("Voltage #6", 5, true));
          v.Add(new Voltage("Voltage #7", 6, true));
          v.Add(new Voltage("3VSB", 7, 34, 34));
          v.Add(new Voltage("VBAT", 8, 34, 34));
          t.Add(new Temperature("CPU Core", 0));
          t.Add(new Temperature("Temperature #1", 1));
          t.Add(new Temperature("Temperature #2", 2));
          t.Add(new Temperature("Temperature #3", 3));
          for (int i = 0; i < superIO.Fans.Length; i++)
            f.Add(new Fan("Fan #" + (i + 1), i));
          for (int i = 0; i < superIO.Controls.Length; i++)
            c.Add(new Ctrl("Fan Control #" + (i + 1), i));
          break;
      }
    }

    private static void GetNuvotonConfigurationD(ISuperIO superIO,
      Manufacturer manufacturer, Model model,
      IList<Voltage> v, IList<Temperature> t, IList<Fan> f, IList<Ctrl> c) 
    {
      switch (manufacturer) {
        case Manufacturer.ASUS:
          switch (model) {
            case Model.P8Z77_V: // NCT6779D
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("Voltage #2", 1, true));
              v.Add(new Voltage("AVCC", 2, 34, 34));
              v.Add(new Voltage("3VCC", 3, 34, 34));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("3VSB", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              v.Add(new Voltage("VTT", 9));
              v.Add(new Voltage("Voltage #11", 10, true));
              v.Add(new Voltage("Voltage #12", 11, true));
              v.Add(new Voltage("Voltage #13", 12, true));
              v.Add(new Voltage("Voltage #14", 13, true));
              v.Add(new Voltage("Voltage #15", 14, true));
              t.Add(new Temperature("CPU Core", 0));
              t.Add(new Temperature("Auxiliary", 1));
              t.Add(new Temperature("Motherboard", 2));
              f.Add(new Fan("Chassis Fan #1", 0));
              f.Add(new Fan("CPU Fan", 1));
              f.Add(new Fan("Chassis Fan #2", 2));
              f.Add(new Fan("Chassis Fan #3", 3));
              c.Add(new Ctrl("Chassis Fan #1", 0));
              c.Add(new Ctrl("CPU  Fan", 1));
              c.Add(new Ctrl("Chassis Fan #2", 2));
              c.Add(new Ctrl("Chassis Fan #3", 3));
              break;
            default:
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("Voltage #2", 1, true));
              v.Add(new Voltage("AVCC", 2, 34, 34));
              v.Add(new Voltage("3VCC", 3, 34, 34));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("3VSB", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              v.Add(new Voltage("VTT", 9));
              v.Add(new Voltage("Voltage #11", 10, true));
              v.Add(new Voltage("Voltage #12", 11, true));
              v.Add(new Voltage("Voltage #13", 12, true));
              v.Add(new Voltage("Voltage #14", 13, true));
              v.Add(new Voltage("Voltage #15", 14, true));
              t.Add(new Temperature("CPU Core", 0));
              t.Add(new Temperature("Temperature #1", 1));
              t.Add(new Temperature("Temperature #2", 2));
              t.Add(new Temperature("Temperature #3", 3));
              t.Add(new Temperature("Temperature #4", 4));
              t.Add(new Temperature("Temperature #5", 5));
              t.Add(new Temperature("Temperature #6", 6));
              for (int i = 0; i < superIO.Fans.Length; i++)
                f.Add(new Fan("Fan #" + (i + 1), i));
              for (int i = 0; i < superIO.Controls.Length; i++)
                c.Add(new Ctrl("Fan Control #" + (i + 1), i));
              break;
          }
          break;
        default:
          v.Add(new Voltage("CPU VCore", 0));
          v.Add(new Voltage("Voltage #2", 1, true));
          v.Add(new Voltage("AVCC", 2, 34, 34));
          v.Add(new Voltage("3VCC", 3, 34, 34));
          v.Add(new Voltage("Voltage #5", 4, true));
          v.Add(new Voltage("Voltage #6", 5, true));
          v.Add(new Voltage("Voltage #7", 6, true));
          v.Add(new Voltage("3VSB", 7, 34, 34));
          v.Add(new Voltage("VBAT", 8, 34, 34));
          v.Add(new Voltage("VTT", 9));
          v.Add(new Voltage("Voltage #11", 10, true));
          v.Add(new Voltage("Voltage #12", 11, true));
          v.Add(new Voltage("Voltage #13", 12, true));
          v.Add(new Voltage("Voltage #14", 13, true));
          v.Add(new Voltage("Voltage #15", 14, true));
          t.Add(new Temperature("CPU Core", 0));
          t.Add(new Temperature("Temperature #1", 1));
          t.Add(new Temperature("Temperature #2", 2));
          t.Add(new Temperature("Temperature #3", 3));
          t.Add(new Temperature("Temperature #4", 4));
          t.Add(new Temperature("Temperature #5", 5));
          t.Add(new Temperature("Temperature #6", 6));
          for (int i = 0; i < superIO.Fans.Length; i++)
            f.Add(new Fan("Fan #" + (i + 1), i));
          for (int i = 0; i < superIO.Controls.Length; i++)
            c.Add(new Ctrl("Fan Control #" + (i + 1), i));
          break;
      }
    }

    private static void GetWinbondConfigurationEHF(Manufacturer manufacturer,
      Model model, IList<Voltage> v, IList<Temperature> t, IList<Fan> f) 
    {
      switch (manufacturer) {
        case Manufacturer.ASRock:
          switch (model) {
            case Model.AOD790GX_128M: // W83627EHF
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("Analog +3.3V", 2, 34, 34));
              v.Add(new Voltage("+3.3V", 4, 10, 10));
              v.Add(new Voltage("+5V", 5, 20, 10));
              v.Add(new Voltage("+12V", 6, 28, 5));
              v.Add(new Voltage("Standby +3.3V", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("Motherboard", 2));
              f.Add(new Fan("CPU Fan", 0));
              f.Add(new Fan("Chassis Fan", 1));
              break;
            default:
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("Voltage #2", 1, true));
              v.Add(new Voltage("AVCC", 2, 34, 34));
              v.Add(new Voltage("3VCC", 3, 34, 34));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("3VSB", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              v.Add(new Voltage("Voltage #10", 9, true));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("Auxiliary", 1));
              t.Add(new Temperature("System", 2));
              f.Add(new Fan("System Fan", 0));
              f.Add(new Fan("CPU Fan", 1));
              f.Add(new Fan("Auxiliary Fan", 2));
              f.Add(new Fan("CPU Fan #2", 3));
              f.Add(new Fan("Auxiliary Fan #2", 4));
              break;
          } break;
        default:
          v.Add(new Voltage("CPU VCore", 0));
          v.Add(new Voltage("Voltage #2", 1, true));
          v.Add(new Voltage("AVCC", 2, 34, 34));
          v.Add(new Voltage("3VCC", 3, 34, 34));
          v.Add(new Voltage("Voltage #5", 4, true));
          v.Add(new Voltage("Voltage #6", 5, true));
          v.Add(new Voltage("Voltage #7", 6, true));
          v.Add(new Voltage("3VSB", 7, 34, 34));
          v.Add(new Voltage("VBAT", 8, 34, 34));
          v.Add(new Voltage("Voltage #10", 9, true));
          t.Add(new Temperature("CPU", 0));
          t.Add(new Temperature("Auxiliary", 1));
          t.Add(new Temperature("System", 2));
          f.Add(new Fan("System Fan", 0));
          f.Add(new Fan("CPU Fan", 1));
          f.Add(new Fan("Auxiliary Fan", 2));
          f.Add(new Fan("CPU Fan #2", 3));
          f.Add(new Fan("Auxiliary Fan #2", 4));
          break;
      }
    }

    private static void GetWinbondConfigurationHG(Manufacturer manufacturer,
      Model model, IList<Voltage> v, IList<Temperature> t, IList<Fan> f) 
    {
      switch (manufacturer) {
        case Manufacturer.ASRock:
          switch (model) {
            case Model._880GMH_USB3: // W83627DHG-P
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("+3.3V", 3, 34, 34));
              v.Add(new Voltage("+5V", 5, 15, 7.5f));
              v.Add(new Voltage("+12V", 6, 56, 10));
              v.Add(new Voltage("Standby +3.3V", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("Motherboard", 2));
              f.Add(new Fan("Chassis Fan", 0));
              f.Add(new Fan("CPU Fan", 1));
              f.Add(new Fan("Power Fan", 2));
              break;
            default:
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("Voltage #2", 1, true));
              v.Add(new Voltage("AVCC", 2, 34, 34));
              v.Add(new Voltage("3VCC", 3, 34, 34));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("3VSB", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("Auxiliary", 1));
              t.Add(new Temperature("System", 2));
              f.Add(new Fan("System Fan", 0));
              f.Add(new Fan("CPU Fan", 1));
              f.Add(new Fan("Auxiliary Fan", 2));
              f.Add(new Fan("CPU Fan #2", 3));
              f.Add(new Fan("Auxiliary Fan #2", 4));
              break;
          }
          break;
        case Manufacturer.ASUS:
          switch (model) {
            case Model.P6T: // W83667HG
            case Model.P6X58D_E: // W83667HG                 
            case Model.Rampage_II_GENE: // W83667HG 
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("+12V", 1, 11.5f, 1.91f));
              v.Add(new Voltage("Analog +3.3V", 2, 34, 34));
              v.Add(new Voltage("+3.3V", 3, 34, 34));
              v.Add(new Voltage("+5V", 4, 15, 7.5f));
              v.Add(new Voltage("Standby +3.3V", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("Motherboard", 2));
              f.Add(new Fan("Chassis Fan #1", 0));
              f.Add(new Fan("CPU Fan", 1));
              f.Add(new Fan("Power Fan", 2));
              f.Add(new Fan("Chassis Fan #2", 3));
              f.Add(new Fan("Chassis Fan #3", 4));
              break;
            case Model.Rampage_Extreme: // W83667HG 
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("+12V", 1, 12, 2));
              v.Add(new Voltage("Analog +3.3V", 2, 34, 34));
              v.Add(new Voltage("+3.3V", 3, 34, 34));
              v.Add(new Voltage("+5V", 4, 15, 7.5f));
              v.Add(new Voltage("Standby +3.3V", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("Motherboard", 2));
              f.Add(new Fan("Chassis Fan #1", 0));
              f.Add(new Fan("CPU Fan", 1));
              f.Add(new Fan("Power Fan", 2));
              f.Add(new Fan("Chassis Fan #2", 3));
              f.Add(new Fan("Chassis Fan #3", 4));
              break;
            default:
              v.Add(new Voltage("CPU VCore", 0));
              v.Add(new Voltage("Voltage #2", 1, true));
              v.Add(new Voltage("AVCC", 2, 34, 34));
              v.Add(new Voltage("3VCC", 3, 34, 34));
              v.Add(new Voltage("Voltage #5", 4, true));
              v.Add(new Voltage("Voltage #6", 5, true));
              v.Add(new Voltage("Voltage #7", 6, true));
              v.Add(new Voltage("3VSB", 7, 34, 34));
              v.Add(new Voltage("VBAT", 8, 34, 34));
              t.Add(new Temperature("CPU", 0));
              t.Add(new Temperature("Auxiliary", 1));
              t.Add(new Temperature("System", 2));
              f.Add(new Fan("System Fan", 0));
              f.Add(new Fan("CPU Fan", 1));
              f.Add(new Fan("Auxiliary Fan", 2));
              f.Add(new Fan("CPU Fan #2", 3));
              f.Add(new Fan("Auxiliary Fan #2", 4));
              break;
          }
          break;
        default:
          v.Add(new Voltage("CPU VCore", 0));
          v.Add(new Voltage("Voltage #2", 1, true));
          v.Add(new Voltage("AVCC", 2, 34, 34));
          v.Add(new Voltage("3VCC", 3, 34, 34));
          v.Add(new Voltage("Voltage #5", 4, true));
          v.Add(new Voltage("Voltage #6", 5, true));
          v.Add(new Voltage("Voltage #7", 6, true));
          v.Add(new Voltage("3VSB", 7, 34, 34));
          v.Add(new Voltage("VBAT", 8, 34, 34));
          t.Add(new Temperature("CPU", 0));
          t.Add(new Temperature("Auxiliary", 1));
          t.Add(new Temperature("System", 2));
          f.Add(new Fan("System Fan", 0));
          f.Add(new Fan("CPU Fan", 1));
          f.Add(new Fan("Auxiliary Fan", 2));
          f.Add(new Fan("CPU Fan #2", 3));
          f.Add(new Fan("Auxiliary Fan #2", 4));
          break;
      }
    }

    public override HardwareType HardwareType {
      get { return HardwareType.SuperIO; }
    }

    public override IHardware Parent {
      get { return mainboard; }
    }


    public override string GetReport() {
      return superIO.GetReport();
    }

    public override void Update() {
      superIO.Update();

      foreach (Sensor sensor in voltages) {
        float? value = readVoltage(sensor.Index);
        if (value.HasValue) {
          sensor.Value = value + (value - sensor.Parameters[2].Value) *
            sensor.Parameters[0].Value / sensor.Parameters[1].Value;
          ActivateSensor(sensor);
        }
      }

      foreach (Sensor sensor in temperatures) {
        float? value = readTemperature(sensor.Index);
        if (value.HasValue) {
          sensor.Value = value + sensor.Parameters[0].Value;
          ActivateSensor(sensor);
        }
      }

      foreach (Sensor sensor in fans) {
        float? value = readFan(sensor.Index);
        if (value.HasValue) {
          sensor.Value = value;
          if (value.Value > 0)
            ActivateSensor(sensor);
        }
      }

      foreach (Sensor sensor in controls) {
        float? value = readControl(sensor.Index);
        sensor.Value = value;                
      }

      postUpdate();
    }

    public override void Close() {
      foreach (Sensor sensor in controls) {
        // restore all controls back to default
        superIO.SetControl(sensor.Index, null);
      }
      base.Close();
    }

    private class Voltage {
      public readonly string Name;
      public readonly int Index;
      public readonly float Ri;
      public readonly float Rf;
      public readonly float Vf;
      public readonly bool Hidden;

      public Voltage(string name, int index) :
        this(name, index, false) { }
      
      public Voltage(string name, int index, bool hidden) :
        this(name, index, 0, 1, 0, hidden) { }
      
      public Voltage(string name, int index, float ri, float rf) :
        this(name, index, ri, rf, 0, false) { }
      
      // float ri = 0, float rf = 1, float vf = 0, bool hidden = false) 
      
      public Voltage(string name, int index, 
        float ri, float rf, float vf, bool hidden) 
      {
        this.Name = name;
        this.Index = index;
        this.Ri = ri;
        this.Rf = rf;
        this.Vf = vf;
        this.Hidden = hidden;
      }
    }

    private class Temperature {
      public readonly string Name;
      public readonly int Index;

      public Temperature(string name, int index) {
        this.Name = name;
        this.Index = index;
      }
    }

    private class Fan {
      public readonly string Name;
      public readonly int Index;

      public Fan(string name, int index) {
        this.Name = name;
        this.Index = index;
      }
    }

    private class Ctrl {
      public readonly string Name;
      public readonly int Index;

      public Ctrl(string name, int index) {
        this.Name = name;
        this.Index = index;
      }
    }
  }
}
