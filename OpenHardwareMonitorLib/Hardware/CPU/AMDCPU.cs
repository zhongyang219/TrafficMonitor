/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2010 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

namespace OpenHardwareMonitor.Hardware.CPU {

  internal abstract class AMDCPU : GenericCPU {

    private const byte PCI_BUS = 0;
    private const byte PCI_BASE_DEVICE = 0x18;
    private const byte DEVICE_VENDOR_ID_REGISTER = 0;
    private const ushort AMD_VENDOR_ID = 0x1022;

    public AMDCPU(int processorIndex, CPUID[][] cpuid, ISettings settings)
      : base(processorIndex, cpuid, settings) { }

    protected uint GetPciAddress(byte function, ushort deviceId) {
      
      // assemble the pci address
      uint address = Ring0.GetPciAddress(PCI_BUS,
        (byte)(PCI_BASE_DEVICE + processorIndex), function);

      // verify that we have the correct bus, device and function
      uint deviceVendor;
      if (!Ring0.ReadPciConfig(
        address, DEVICE_VENDOR_ID_REGISTER, out deviceVendor))
        return Ring0.InvalidPciAddress;
      
      if (deviceVendor != (deviceId << 16 | AMD_VENDOR_ID))
        return Ring0.InvalidPciAddress;

      return address;
    }

  }
}
