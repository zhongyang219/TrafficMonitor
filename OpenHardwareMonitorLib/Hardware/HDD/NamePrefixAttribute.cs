/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2011 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Collections.Generic;

namespace OpenHardwareMonitor.Hardware.HDD {

  [AttributeUsage(AttributeTargets.Class, AllowMultiple = true)]
  internal class NamePrefixAttribute : Attribute {

    public NamePrefixAttribute(string namePrefix) {
      Prefix = namePrefix;
    }

    public string Prefix { get; private set; }

  }
}
