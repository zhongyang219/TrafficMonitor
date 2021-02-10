/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2010-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	Copyright (C) 2015 Dawid Gan <deveee@gmail.com>

*/

using System;
using System.Globalization;
using System.Text;

namespace OpenHardwareMonitor.Hardware.LPC {
  internal class NCT677X : ISuperIO {

    private readonly ushort port;
    private readonly byte revision;

    private readonly Chip chip;
    private readonly LPCPort lpcPort;

    private readonly bool isNuvotonVendor;

    private readonly float?[] voltages = new float?[0];
    private readonly float?[] temperatures = new float?[0];
    private readonly float?[] fans = new float?[0];
    private readonly float?[] controls = new float?[0];

    // Hardware Monitor
    private const uint ADDRESS_REGISTER_OFFSET = 0x05;
    private const uint DATA_REGISTER_OFFSET = 0x06;
    private const byte BANK_SELECT_REGISTER = 0x4E;

    private byte ReadByte(ushort address) {
      byte bank = (byte)(address >> 8);
      byte register = (byte)(address & 0xFF);
      Ring0.WriteIoPort(port + ADDRESS_REGISTER_OFFSET, BANK_SELECT_REGISTER);
      Ring0.WriteIoPort(port + DATA_REGISTER_OFFSET, bank);
      Ring0.WriteIoPort(port + ADDRESS_REGISTER_OFFSET, register);
      return Ring0.ReadIoPort(port + DATA_REGISTER_OFFSET);
    }

    private void WriteByte(ushort address, byte value) {
      byte bank = (byte)(address >> 8);
      byte register = (byte)(address & 0xFF);
      Ring0.WriteIoPort(port + ADDRESS_REGISTER_OFFSET, BANK_SELECT_REGISTER);
      Ring0.WriteIoPort(port + DATA_REGISTER_OFFSET, bank);
      Ring0.WriteIoPort(port + ADDRESS_REGISTER_OFFSET, register);
      Ring0.WriteIoPort(port + DATA_REGISTER_OFFSET, value);
    }

    // Consts 
    private const ushort NUVOTON_VENDOR_ID = 0x5CA3;

    // Hardware Monitor Registers    
    private readonly ushort VENDOR_ID_HIGH_REGISTER;
    private readonly ushort VENDOR_ID_LOW_REGISTER;  
    
    private readonly ushort[] FAN_PWM_OUT_REG;
    private readonly ushort[] FAN_PWM_COMMAND_REG;
    private readonly ushort[] FAN_CONTROL_MODE_REG;

    private readonly ushort[] fanRpmBaseRegister;
    private readonly int minFanRPM;

    private readonly ushort[] fanCountRegister;
    private readonly int maxFanCount;
    private readonly int minFanCount;

    private bool[] restoreDefaultFanControlRequired = new bool[7];
    private byte[] initialFanControlMode = new byte[7];
    private byte[] initialFanPwmCommand = new byte[7];

    private readonly ushort[] voltageRegisters;
    private readonly ushort voltageVBatRegister;
    private readonly ushort vBatMonitorControlRegister;

    private readonly byte[] temperaturesSource;

    private readonly ushort[] temperatureRegister;
    private readonly ushort[] temperatureHalfRegister;
    private readonly int[] temperatureHalfBit;  
    private readonly ushort[] temperatureSourceRegister;        

    private readonly ushort?[] alternateTemperatureRegister;

    private enum SourceNCT6771F : byte {
      SYSTIN = 1,
      CPUTIN = 2,
      AUXTIN = 3,
      SMBUSMASTER = 4,
      PECI_0 = 5, 
      PECI_1 = 6, 
      PECI_2 = 7,
      PECI_3 = 8,
      PECI_4 = 9,
      PECI_5 = 10,
      PECI_6 = 11,
      PECI_7 = 12,
      PCH_CHIP_CPU_MAX_TEMP = 13,
      PCH_CHIP_TEMP = 14,
      PCH_CPU_TEMP = 15,
      PCH_MCH_TEMP = 16, 
      PCH_DIM0_TEMP = 17,
      PCH_DIM1_TEMP = 18,
      PCH_DIM2_TEMP = 19,
      PCH_DIM3_TEMP = 20
    }

    private enum SourceNCT6776F : byte {
      SYSTIN = 1,
      CPUTIN = 2,
      AUXTIN = 3,
      SMBUSMASTER_0 = 4,
      SMBUSMASTER_1 = 5,
      SMBUSMASTER_2 = 6,
      SMBUSMASTER_3 = 7,
      SMBUSMASTER_4 = 8,
      SMBUSMASTER_5 = 9,
      SMBUSMASTER_6 = 10,
      SMBUSMASTER_7 = 11,
      PECI_0 = 12,
      PECI_1 = 13,
      PCH_CHIP_CPU_MAX_TEMP = 14,
      PCH_CHIP_TEMP = 15,
      PCH_CPU_TEMP = 16,
      PCH_MCH_TEMP = 17,
      PCH_DIM0_TEMP = 18,
      PCH_DIM1_TEMP = 19,
      PCH_DIM2_TEMP = 20,
      PCH_DIM3_TEMP = 21,
      BYTE_TEMP = 22
    }

    private enum SourceNCT67XXD : byte {
      SYSTIN = 1,
      CPUTIN = 2,
      AUXTIN0 = 3,
      AUXTIN1 = 4,
      AUXTIN2 = 5,
      AUXTIN3 = 6,      
      SMBUSMASTER_0 = 8,
      SMBUSMASTER_1 = 9,
      SMBUSMASTER_2 = 10,
      SMBUSMASTER_3 = 11,
      SMBUSMASTER_4 = 12,
      SMBUSMASTER_5 = 13,
      SMBUSMASTER_6 = 14,
      SMBUSMASTER_7 = 15,
      PECI_0 = 16,
      PECI_1 = 17,
      PCH_CHIP_CPU_MAX_TEMP = 18,
      PCH_CHIP_TEMP = 19,
      PCH_CPU_TEMP = 20,
      PCH_MCH_TEMP = 21,
      PCH_DIM0_TEMP = 22,
      PCH_DIM1_TEMP = 23,
      PCH_DIM2_TEMP = 24,
      PCH_DIM3_TEMP = 25,
      BYTE_TEMP = 26
    }

    private enum SourceNCT610X : byte {
      SYSTIN = 1,
      CPUTIN = 2,
      AUXTIN = 3,
      SMBUSMASTER_0 = 4,
      SMBUSMASTER_1 = 5,
      SMBUSMASTER_2 = 6,
      SMBUSMASTER_3 = 7,
      SMBUSMASTER_4 = 8,
      SMBUSMASTER_5 = 9,
      SMBUSMASTER_6 = 10,
      SMBUSMASTER_7 = 11,
      PECI_0 = 12,
      PECI_1 = 13,
      PCH_CHIP_CPU_MAX_TEMP = 14,
      PCH_CHIP_TEMP = 15,
      PCH_CPU_TEMP = 16,
      PCH_MCH_TEMP = 17,
      PCH_DIM0_TEMP = 18,
      PCH_DIM1_TEMP = 19,
      PCH_DIM2_TEMP = 20,
      PCH_DIM3_TEMP = 21,
      BYTE_TEMP = 22
    }

    public NCT677X(Chip chip, byte revision, ushort port, LPCPort lpcPort) 
  {
      this.chip = chip;
      this.revision = revision;
      this.port = port;
      this.lpcPort = lpcPort;

      if (chip == LPC.Chip.NCT610X) {
        VENDOR_ID_HIGH_REGISTER = 0x80FE;
        VENDOR_ID_LOW_REGISTER = 0x00FE;  

        FAN_PWM_OUT_REG = new ushort[] { 0x04A, 0x04B, 0x04C };
        FAN_PWM_COMMAND_REG = new ushort[] { 0x119, 0x129, 0x139 };
        FAN_CONTROL_MODE_REG = new ushort[] { 0x113, 0x123, 0x133 };

        vBatMonitorControlRegister = 0x0318;
      } else {
        VENDOR_ID_HIGH_REGISTER = 0x804F;
        VENDOR_ID_LOW_REGISTER = 0x004F;  

        FAN_PWM_OUT_REG = new ushort[] { 
          0x001, 0x003, 0x011, 0x013, 0x015, 0x017, 0x029 };
        FAN_PWM_COMMAND_REG = new ushort[] { 
          0x109, 0x209, 0x309, 0x809, 0x909, 0xA09, 0xB09 };
        FAN_CONTROL_MODE_REG = new ushort[] { 
          0x102, 0x202, 0x302, 0x802, 0x902, 0xA02, 0xB02 };

        vBatMonitorControlRegister = 0x005D;
      }

      this.isNuvotonVendor = IsNuvotonVendor();

      if (!isNuvotonVendor)
        return;

      switch (chip) {
        case Chip.NCT6771F:
        case Chip.NCT6776F:          
          if (chip == Chip.NCT6771F) {
            fans = new float?[4];

            // min RPM value with 16-bit fan counter
            minFanRPM = (int)(1.35e6 / 0xFFFF);

            temperaturesSource = new byte[] {
              (byte)SourceNCT6771F.PECI_0,
              (byte)SourceNCT6771F.CPUTIN,
              (byte)SourceNCT6771F.AUXTIN,
              (byte)SourceNCT6771F.SYSTIN
            };            
          } else {
            fans = new float?[5];

            // min RPM value with 13-bit fan counter
            minFanRPM = (int)(1.35e6 / 0x1FFF);

            temperaturesSource = new byte[] {
              (byte)SourceNCT6776F.PECI_0,
              (byte)SourceNCT6776F.CPUTIN,
              (byte)SourceNCT6776F.AUXTIN,
              (byte)SourceNCT6776F.SYSTIN 
            };
          }
          fanRpmBaseRegister = new ushort[]
            { 0x656, 0x658, 0x65A, 0x65C, 0x65E };

          controls = new float?[3];

          voltages = new float?[9];
          voltageRegisters = new ushort[] 
            { 0x020, 0x021, 0x022, 0x023, 0x024, 0x025, 0x026, 0x550, 0x551 };
          voltageVBatRegister = 0x551;

          temperatures = new float?[4];
          temperatureRegister = new ushort[]
            { 0x027, 0x073, 0x075, 0x077, 0x150, 0x250, 0x62B, 0x62C, 0x62D };
          temperatureHalfRegister = new ushort[]
            { 0, 0x074, 0x076, 0x078, 0x151, 0x251, 0x62E, 0x62E, 0x62E };
          temperatureHalfBit = new int[] 
            { -1, 7, 7, 7, 7, 7, 0, 1, 2 };
          temperatureSourceRegister = new ushort[]
            { 0x621, 0x100, 0x200, 0x300, 0x622, 0x623, 0x624, 0x625, 0x626 };

          alternateTemperatureRegister = new ushort?[] 
            { null, null, null, null };
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
          switch (chip) {
            case Chip.NCT6791D:
            case Chip.NCT6792D:
            case Chip.NCT6792DA:
            case Chip.NCT6793D:
            case Chip.NCT6795D:
              fans = new float?[6];
              controls = new float?[6];
              break;
            case Chip.NCT6796D:
            case Chip.NCT6796DR:
            case Chip.NCT6797D:
            case Chip.NCT6798D:
              fans = new float?[7];
              controls = new float?[7];
              break;
            default:
              fans = new float?[5];
              controls = new float?[5];
              break;
          }

          fanCountRegister = new ushort[]
            { 0x4B0, 0x4B2, 0x4B4, 0x4B6, 0x4B8, 0x4BA, 0x4CC };

          // max value for 13-bit fan counter
          maxFanCount = 0x1FFF;

          // min value that could be transfered to 16-bit RPM registers
          minFanCount = 0x15;

          voltages = new float?[15];
          voltageRegisters = new ushort[] 
            { 0x480, 0x481, 0x482, 0x483, 0x484, 0x485, 0x486, 0x487, 0x488, 
              0x489, 0x48A, 0x48B, 0x48C, 0x48D, 0x48E };
          voltageVBatRegister = 0x488;

          temperatures = new float?[7];
          temperaturesSource = new byte[] {
            (byte)SourceNCT67XXD.PECI_0,
            (byte)SourceNCT67XXD.CPUTIN,
            (byte)SourceNCT67XXD.SYSTIN,
            (byte)SourceNCT67XXD.AUXTIN0,
            (byte)SourceNCT67XXD.AUXTIN1,
            (byte)SourceNCT67XXD.AUXTIN2,
            (byte)SourceNCT67XXD.AUXTIN3
          };

          temperatureRegister = new ushort[]
            { 0x027, 0x073, 0x075, 0x077, 0x079, 0x07B, 0x150 };
          temperatureHalfRegister = new ushort[]
            { 0, 0x074, 0x076, 0x078, 0x07A, 0x07C, 0x151 };              
          temperatureHalfBit = new int[]
            { -1, 7, 7, 7, 7, 7, 7 };
          temperatureSourceRegister = new ushort[] 
            { 0x621, 0x100, 0x200, 0x300, 0x800, 0x900, 0x622 };

          alternateTemperatureRegister = new ushort?[] 
            {null, 0x491, 0x490, 0x492, 0x493, 0x494, 0x495 };

          break;
        case Chip.NCT610X:

          fans = new float?[3];
          controls = new float?[3];

          fanRpmBaseRegister = new ushort[] { 0x030, 0x032, 0x034 };

          // min value RPM value with 13-bit fan counter
          minFanRPM = (int)(1.35e6 / 0x1FFF);

          voltages = new float?[9];
          voltageRegisters = new ushort[] 
            { 0x300, 0x301, 0x302, 0x303, 0x304, 0x305, 0x307, 0x308, 0x309 };
          voltageVBatRegister = 0x308;

          temperatures = new float?[4];
          temperaturesSource = new byte[] {
            (byte)SourceNCT610X.PECI_0,
            (byte)SourceNCT610X.SYSTIN,
            (byte)SourceNCT610X.CPUTIN,
            (byte)SourceNCT610X.AUXTIN
        };

        temperatureRegister = new ushort[]
        { 0x027, 0x018, 0x019, 0x01A };
        temperatureHalfRegister = new ushort[]
        { 0, 0x01B, 0x11B, 0x21B };              
        temperatureHalfBit = new int[]
        { -1, 7, 7, 7 };
        temperatureSourceRegister = new ushort[] 
        { 0x621, 0x100, 0x200, 0x300 };

        alternateTemperatureRegister = new ushort?[] 
        {null, 0x018, 0x019, 0x01A };

        break;
      }
    }

    private bool IsNuvotonVendor() {
      return ((ReadByte(VENDOR_ID_HIGH_REGISTER) << 8) |
        ReadByte(VENDOR_ID_LOW_REGISTER)) == NUVOTON_VENDOR_ID;
    }

    public byte? ReadGPIO(int index) {
      return null;
    }

    public void WriteGPIO(int index, byte value) { }


    private void SaveDefaultFanControl(int index) {
      if (!restoreDefaultFanControlRequired[index]) {
        initialFanControlMode[index] = ReadByte(FAN_CONTROL_MODE_REG[index]);
        initialFanPwmCommand[index] = ReadByte(FAN_PWM_COMMAND_REG[index]);
        restoreDefaultFanControlRequired[index] = true;
      }
    }

    private void RestoreDefaultFanControl(int index) {
      if (restoreDefaultFanControlRequired[index]) {
        WriteByte(FAN_CONTROL_MODE_REG[index], initialFanControlMode[index]);
        WriteByte(FAN_PWM_COMMAND_REG[index], initialFanPwmCommand[index]);
        restoreDefaultFanControlRequired[index] = false;
      }
    }

    public void SetControl(int index, byte? value) {
      if (!isNuvotonVendor)
        return;

      if (index < 0 || index >= controls.Length)
        throw new ArgumentOutOfRangeException("index");

      if (!Ring0.WaitIsaBusMutex(10))
        return;

      if (value.HasValue) {
        SaveDefaultFanControl(index);

        // set manual mode
        WriteByte(FAN_CONTROL_MODE_REG[index], 0);

        // set output value
        WriteByte(FAN_PWM_COMMAND_REG[index], value.Value);  
      } else {
        RestoreDefaultFanControl(index);
      }

      Ring0.ReleaseIsaBusMutex();
    }   

    public Chip Chip { get { return chip; } }
    public float?[] Voltages { get { return voltages; } }
    public float?[] Temperatures { get { return temperatures; } }
    public float?[] Fans { get { return fans; } }
    public float?[] Controls { get { return controls; } }

    private void DisableIOSpaceLock() {
      if (chip != Chip.NCT6791D && 
          chip != Chip.NCT6792D && 
          chip != Chip.NCT6792DA &&
          chip != Chip.NCT6793D &&
          chip != Chip.NCT6795D &&
          chip != Chip.NCT6796D &&
          chip != Chip.NCT6796DR &&
          chip != Chip.NCT6797D &&
          chip != Chip.NCT6798D)
        return;

      // the lock is disabled already if the vendor ID can be read
      if (IsNuvotonVendor())
        return;

      lpcPort.WinbondNuvotonFintekEnter();
      lpcPort.NuvotonDisableIOSpaceLock();
      lpcPort.WinbondNuvotonFintekExit();
    }

    public void Update() {
      if (!isNuvotonVendor)
        return;

      if (!Ring0.WaitIsaBusMutex(10))
        return;

      DisableIOSpaceLock();

      for (int i = 0; i < voltages.Length; i++) {
        float value = 0.008f * ReadByte(voltageRegisters[i]);
        bool valid = value > 0;

        // check if battery voltage monitor is enabled
        if (valid && voltageRegisters[i] == voltageVBatRegister) 
          valid = (ReadByte(vBatMonitorControlRegister) & 0x01) > 0;

        voltages[i] = valid ? value : (float?)null;
      }

      int temperatureSourceMask = 0;
      for (int i = temperatureRegister.Length - 1; i >= 0 ; i--) {
        int value = ((sbyte)ReadByte(temperatureRegister[i])) << 1;
        if (temperatureHalfBit[i] > 0) {
          value |= ((ReadByte(temperatureHalfRegister[i]) >>
            temperatureHalfBit[i]) & 0x1);
        }

        byte source = ReadByte(temperatureSourceRegister[i]);
        temperatureSourceMask |= 1 << source;

        float? temperature = 0.5f * value;
        if (temperature > 125 || temperature < -55)
          temperature = null;

        for (int j = 0; j < temperatures.Length; j++) 
          if (temperaturesSource[j] == source)
            temperatures[j] = temperature; 
      }
      for (int i = 0; i < alternateTemperatureRegister.Length; i++) {
        if (!alternateTemperatureRegister[i].HasValue)
          continue;

        if ((temperatureSourceMask & (1 << temperaturesSource[i])) > 0)
          continue;

        float? temperature = (sbyte)
          ReadByte(alternateTemperatureRegister[i].Value);

        if (temperature > 125 || temperature < -55)
          temperature = null;

        temperatures[i] = temperature;
      }

      for (int i = 0; i < fans.Length; i++) {
        if (fanCountRegister != null) {
          byte high = ReadByte(fanCountRegister[i]);
          byte low = ReadByte((ushort)(fanCountRegister[i] + 1));
          int count = (high << 5) | (low & 0x1F); 
          if (count < maxFanCount) {            
            if (count >= minFanCount) {
              fans[i] = 1.35e6f / count;
            } else {
              fans[i] = null;
            }
          } else {
            fans[i] = 0;
          }
        } else {
          byte high = ReadByte(fanRpmBaseRegister[i]);
          byte low = ReadByte((ushort)(fanRpmBaseRegister[i] + 1));
          int value = (high << 8) | low;

          fans[i] = value > minFanRPM ? value : 0;
        }
      }

      for (int i = 0; i < controls.Length; i++) {
        int value = ReadByte(FAN_PWM_OUT_REG[i]);
        controls[i] = value / 2.55f;
      }

      Ring0.ReleaseIsaBusMutex();
    }

    public string GetReport() {
      StringBuilder r = new StringBuilder();

      r.AppendLine("LPC " + this.GetType().Name);
      r.AppendLine();
      r.Append("Chip ID: 0x"); r.AppendLine(chip.ToString("X"));
      r.Append("Chip revision: 0x");
      r.AppendLine(revision.ToString("X", CultureInfo.InvariantCulture));
      r.Append("Base Adress: 0x");
      r.AppendLine(port.ToString("X4", CultureInfo.InvariantCulture));
      r.AppendLine();

      if (!Ring0.WaitIsaBusMutex(100))
        return r.ToString();

      ushort[] addresses = new ushort[] { 
        0x000, 0x010, 0x020, 0x030, 0x040, 0x050, 0x060, 0x070, 0x0F0,
        0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 
        0x200, 0x210, 0x220, 0x230, 0x240, 0x250, 0x260,
        0x300,        0x320, 0x330, 0x340,        0x360,
        0x400, 0x410, 0x420,        0x440, 0x450, 0x460, 0x480, 0x490, 0x4B0, 
                                                                0x4C0, 0x4F0,
        0x500,                             0x550, 0x560,
        0x600, 0x610 ,0x620, 0x630, 0x640, 0x650, 0x660, 0x670, 
        0x700, 0x710, 0x720, 0x730,
        0x800,        0x820, 0x830, 0x840,
        0x900,        0x920, 0x930, 0x940,        0x960,
        0xA00, 0xA10, 0xA20, 0xA30, 0xA40, 0xA50, 0xA60, 0xA70, 
        0xB00, 0xB10, 0xB20, 0xB30,        0xB50, 0xB60, 0xB70, 
        0xC00, 0xC10, 0xC20, 0xC30,        0xC50, 0xC60, 0xC70,
        0xD00, 0xD10, 0xD20, 0xD30,        0xD50, 0xD60, 
        0xE00, 0xE10, 0xE20, 0xE30, 
        0xF00, 0xF10, 0xF20, 0xF30,
        0x8040, 0x80F0};

      r.AppendLine("Hardware Monitor Registers");
      r.AppendLine();
      r.AppendLine("        00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
      r.AppendLine();
      foreach (ushort address in addresses) {
          r.Append(" ");
          r.Append(address.ToString("X4", CultureInfo.InvariantCulture));
          r.Append("  ");
          for (ushort j = 0; j <= 0xF; j++) {
            r.Append(" ");
            r.Append(ReadByte((ushort)(address | j)).ToString(
              "X2", CultureInfo.InvariantCulture));
          }
          r.AppendLine();
      }
      r.AppendLine();

      Ring0.ReleaseIsaBusMutex();

      return r.ToString();
    }
  }
}
