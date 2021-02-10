/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2013 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System.Collections;
using System.Collections.Generic;

namespace OpenHardwareMonitor.Collections {

  public class ListSet<T> : IEnumerable<T> {

    private readonly List<T> list = new List<T>();

    public bool Add(T item) {
      if (list.Contains(item))
        return false;

      list.Add(item);
      return true;
    }

    public bool Remove(T item) {
      if (!list.Contains(item))
        return false;

      list.Remove(item);
      return true;
    }

    public bool Contains(T item) {
      return list.Contains(item);
    }

    public T[] ToArray() {
      return list.ToArray();
    }

    public IEnumerator<T> GetEnumerator() {
      return list.GetEnumerator();
    }

    IEnumerator IEnumerable.GetEnumerator() {
      return list.GetEnumerator();
    }

    public int Count {
      get {
        return list.Count;
      }
    }
  }
}
