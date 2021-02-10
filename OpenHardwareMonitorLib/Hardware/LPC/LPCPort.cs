/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

namespace OpenHardwareMonitor.Hardware.LPC {

  internal class LPCPort {
    private readonly ushort registerPort;
    private readonly ushort valuePort;

    public LPCPort(ushort registerPort, ushort valuePort) {
      this.registerPort = registerPort;
      this.valuePort = valuePort;
    }

    public ushort RegisterPort {
      get {
        return registerPort;
      }
    }

    public ushort ValuePort {
      get {
        return valuePort;
      }
    }

    private const byte DEVCIE_SELECT_REGISTER = 0x07;
    private const byte CONFIGURATION_CONTROL_REGISTER = 0x02;

    public byte ReadByte(byte register) {
      Ring0.WriteIoPort(registerPort, register);
      return Ring0.ReadIoPort(valuePort);
    }

    public void WriteByte(byte register, byte value) {
      Ring0.WriteIoPort(registerPort, register);
      Ring0.WriteIoPort(valuePort, value);
    }

    public ushort ReadWord(byte register) {
      return (ushort)((ReadByte(register) << 8) |
        ReadByte((byte)(register + 1)));
    }

    public void Select(byte logicalDeviceNumber) {
      Ring0.WriteIoPort(registerPort, DEVCIE_SELECT_REGISTER);
      Ring0.WriteIoPort(valuePort, logicalDeviceNumber);
    }

    public void WinbondNuvotonFintekEnter() {
      Ring0.WriteIoPort(registerPort, 0x87);
      Ring0.WriteIoPort(registerPort, 0x87);
    }

    public void WinbondNuvotonFintekExit() {
      Ring0.WriteIoPort(registerPort, 0xAA);
    }

    private const byte NUVOTON_HARDWARE_MONITOR_IO_SPACE_LOCK = 0x28;

    public void NuvotonDisableIOSpaceLock() {
      byte options = ReadByte(NUVOTON_HARDWARE_MONITOR_IO_SPACE_LOCK);

      // if the i/o space lock is enabled
      if ((options & 0x10) > 0) {

        // disable the i/o space lock
        WriteByte(NUVOTON_HARDWARE_MONITOR_IO_SPACE_LOCK, 
          (byte)(options & ~0x10));
      }
    }

    public void IT87Enter() {
      Ring0.WriteIoPort(registerPort, 0x87);
      Ring0.WriteIoPort(registerPort, 0x01);
      Ring0.WriteIoPort(registerPort, 0x55);

      if (registerPort == 0x4E) {
        Ring0.WriteIoPort(registerPort, 0xAA);
      } else {
        Ring0.WriteIoPort(registerPort, 0x55);
      }
    }

    public void IT87Exit() {
      // do not exit config mode for secondary super IO
      if (registerPort != 0x4E) {
        Ring0.WriteIoPort(registerPort, CONFIGURATION_CONTROL_REGISTER);
        Ring0.WriteIoPort(valuePort, 0x02);
      }
    }

    public void SMSCEnter() {
      Ring0.WriteIoPort(registerPort, 0x55);
    }

    public void SMSCExit() {
      Ring0.WriteIoPort(registerPort, 0xAA);
    }
  }

}
