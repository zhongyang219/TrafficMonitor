/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2011 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Collections.Generic;

namespace OpenHardwareMonitor.Collections {

  public struct Pair<F, S> {
    private F first;
    private S second;

    public Pair(F first, S second) {
      this.first = first;
      this.second = second;
    }

    public F First {
      get { return first; }
      set { first = value; }
    }

    public S Second {
      get { return second; }
      set { second = value; }
    }

    public override int GetHashCode() {
      return (first != null ? first.GetHashCode() : 0) ^
        (second != null ? second.GetHashCode() : 0);
    }
  }
}
