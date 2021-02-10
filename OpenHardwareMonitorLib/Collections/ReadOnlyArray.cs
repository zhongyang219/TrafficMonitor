/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2010 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System.Collections;
using System.Collections.Generic;

namespace OpenHardwareMonitor.Collections {

  public class ReadOnlyArray<T> : IReadOnlyArray<T> {

    private readonly T[] array;

    public ReadOnlyArray(T[] array) {
      this.array = array;
    }

    public T this[int index] {
      get { return array[index]; }
    }

    public int Length { get { return array.Length; } }

    public IEnumerator<T> GetEnumerator() {
      return ((IEnumerable<T>)array).GetEnumerator();
    }

    IEnumerator IEnumerable.GetEnumerator() {
      return array.GetEnumerator();
    }

    public static implicit operator ReadOnlyArray<T>(T[] array) {
      return new ReadOnlyArray<T>(array);
    }

    public T[] ToArray() {
      return (T[])array.Clone();
    }
  }
}
