/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Collections.Generic;
using System.Text;

namespace OpenHardwareMonitor.Hardware {

  /// <summary>
  /// This structure describes a group-specific affinity.
  /// </summary>
  internal struct GroupAffinity {

    public static GroupAffinity Undefined = 
      new GroupAffinity(ushort.MaxValue, 0);

    public GroupAffinity(ushort group, ulong mask) {
      this.Group = group;
      this.Mask = mask;
    }

    public static GroupAffinity Single(ushort group, int index) {
      return new GroupAffinity(group, 1UL << index);
    }

    public ushort Group { get; }

    public ulong Mask { get; }

    public override bool Equals(object o) {
      if (o == null || GetType() != o.GetType()) return false;
      GroupAffinity a = (GroupAffinity)o;
      return (Group == a.Group) && (Mask == a.Mask);
    }

    public override int GetHashCode() {
      return Group.GetHashCode() ^ Mask.GetHashCode();
    }

    public static bool operator ==(GroupAffinity a1, GroupAffinity a2) {
      return (a1.Group == a2.Group) && (a1.Mask == a2.Mask);
    }

    public static bool operator !=(GroupAffinity a1, GroupAffinity a2) {
      return (a1.Group != a2.Group) || (a1.Mask != a2.Mask);
    }

  }
}
