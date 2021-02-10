/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2010 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

namespace OpenHardwareMonitor.Hardware.Mainboard {
  internal class MainboardGroup : IGroup {

    private readonly Mainboard[] mainboards;

    public MainboardGroup(SMBIOS smbios, ISettings settings) {
      mainboards = new Mainboard[1];
      mainboards[0] = new Mainboard(smbios, settings);
    }

    public void Close() {
      foreach (Mainboard mainboard in mainboards)
        mainboard.Close();
    }

    public string GetReport() {
      return null;
    }

    public IHardware[] Hardware {
      get { return mainboards; }

    }
  }
}
