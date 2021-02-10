/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text;

namespace OpenHardwareMonitor.Hardware.LPC {

  internal class LMSensors {

    private readonly List<LMChip> lmChips = new List<LMChip>();

    public LMSensors() {
      string[] basePaths = Directory.GetDirectories("/sys/class/hwmon/");
      foreach (string basePath in basePaths) {
        foreach (string devicePath in new[] { "/device", "" }) {
          string path = basePath + devicePath;

          string name = null;
          try {
            using (StreamReader reader = new StreamReader(path + "/name"))
              name = reader.ReadLine();
          } catch (IOException) { }

          switch (name) {
            case "atk0110":
              lmChips.Add(new LMChip(Chip.ATK0110, path)); break;

            case "f71858fg":
              lmChips.Add(new LMChip(Chip.F71858, path)); break;
            case "f71862fg":
              lmChips.Add(new LMChip(Chip.F71862, path)); break;
            case "f71869":
              lmChips.Add(new LMChip(Chip.F71869, path)); break;
            case "f71869a":
              lmChips.Add(new LMChip(Chip.F71869A, path)); break;
            case "f71882fg":
              lmChips.Add(new LMChip(Chip.F71882, path)); break;            
            case "f71889a":
              lmChips.Add(new LMChip(Chip.F71889AD, path)); break;            
            case "f71878ad":
              lmChips.Add(new LMChip(Chip.F71878AD, path)); break;
            case "f71889ed":
              lmChips.Add(new LMChip(Chip.F71889ED, path)); break;
            case "f71889fg":
              lmChips.Add(new LMChip(Chip.F71889F, path)); break;
            case "f71808e":
              lmChips.Add(new LMChip(Chip.F71808E, path)); break;

            case "it8705":
              lmChips.Add(new LMChip(Chip.IT8705F, path)); break;
            case "it8712":
              lmChips.Add(new LMChip(Chip.IT8712F, path)); break;
            case "it8716":
              lmChips.Add(new LMChip(Chip.IT8716F, path)); break;
            case "it8718":
              lmChips.Add(new LMChip(Chip.IT8718F, path)); break;
            case "it8720":
              lmChips.Add(new LMChip(Chip.IT8720F, path)); break;
            
            case "nct6775":
              lmChips.Add(new LMChip(Chip.NCT6771F, path)); break;
            case "nct6776":
              lmChips.Add(new LMChip(Chip.NCT6776F, path)); break;
            case "nct6779":
              lmChips.Add(new LMChip(Chip.NCT6779D, path)); break;
            case "nct6791":
              lmChips.Add(new LMChip(Chip.NCT6791D, path)); break;
            case "nct6792":
              lmChips.Add(new LMChip(Chip.NCT6792D, path)); break;
            case "nct6793":
              lmChips.Add(new LMChip(Chip.NCT6793D, path)); break;
            case "nct6795":
              lmChips.Add(new LMChip(Chip.NCT6795D, path)); break;
            case "nct6796":
              lmChips.Add(new LMChip(Chip.NCT6796D, path)); break;
            case "nct6797":
              lmChips.Add(new LMChip(Chip.NCT6797D, path)); break;
            case "nct6798":
              lmChips.Add(new LMChip(Chip.NCT6798D, path)); break;

            case "w83627ehf":
              lmChips.Add(new LMChip(Chip.W83627EHF, path)); break;
            case "w83627dhg":
              lmChips.Add(new LMChip(Chip.W83627DHG, path)); break;
            case "w83667hg":
              lmChips.Add(new LMChip(Chip.W83667HG, path)); break;
            case "w83627hf":
              lmChips.Add(new LMChip(Chip.W83627HF, path)); break;
            case "w83627thf":
              lmChips.Add(new LMChip(Chip.W83627THF, path)); break;
            case "w83687thf":
              lmChips.Add(new LMChip(Chip.W83687THF, path)); break;
          }
        }
      }
    }

    public void Close() {
      foreach (LMChip lmChip in lmChips)
        lmChip.Close();
    }

    public ISuperIO[] SuperIO {
      get {
        return lmChips.ToArray();
      }
    }

    private class LMChip : ISuperIO {

      private string path;
      private readonly Chip chip;

      private readonly float?[] voltages;
      private readonly float?[] temperatures;
      private readonly float?[] fans;
      private readonly float?[] controls;

      private readonly FileStream[] voltageStreams;
      private readonly FileStream[] temperatureStreams;
      private readonly FileStream[] fanStreams;

      public Chip Chip { get { return chip; } }
      public float?[] Voltages { get { return voltages; } }
      public float?[] Temperatures { get { return temperatures; } }
      public float?[] Fans { get { return fans; } }
      public float?[] Controls { get { return controls; } }

      public LMChip(Chip chip, string path) {
        this.path = path;
        this.chip = chip;

        string[] voltagePaths = Directory.GetFiles(path, "in*_input");
        this.voltages = new float?[voltagePaths.Length];
        this.voltageStreams = new FileStream[voltagePaths.Length];
        for (int i = 0; i < voltagePaths.Length; i++)
          voltageStreams[i] = new FileStream(voltagePaths[i],
            FileMode.Open, FileAccess.Read, FileShare.ReadWrite);

        string[] temperaturePaths = Directory.GetFiles(path, "temp*_input");
        this.temperatures = new float?[temperaturePaths.Length];
        this.temperatureStreams = new FileStream[temperaturePaths.Length];
        for (int i = 0; i < temperaturePaths.Length; i++)
          temperatureStreams[i] = new FileStream(temperaturePaths[i],
            FileMode.Open, FileAccess.Read, FileShare.ReadWrite);

        string[] fanPaths = Directory.GetFiles(path, "fan*_input");
        this.fans = new float?[fanPaths.Length];
        this.fanStreams = new FileStream[fanPaths.Length];
        for (int i = 0; i < fanPaths.Length; i++)
          fanStreams[i] = new FileStream(fanPaths[i],
            FileMode.Open, FileAccess.Read, FileShare.ReadWrite);

        this.controls = new float?[0];
      }

      public byte? ReadGPIO(int index) {
        return null;
      }

      public void WriteGPIO(int index, byte value) { }

      public string GetReport() {
        return null;
      }

      public void SetControl(int index, byte? value) { }   

      private string ReadFirstLine(Stream stream) {
        StringBuilder sb = new StringBuilder();
        try {
          stream.Seek(0, SeekOrigin.Begin);
          int b = stream.ReadByte();
          while (b != -1 && b != 10) {
            sb.Append((char)b);
            b = stream.ReadByte();
          }
        } catch { }
        return sb.ToString();
      }

      public void Update() {
        for (int i = 0; i < voltages.Length; i++) {
          string s = ReadFirstLine(voltageStreams[i]);
          try {
            voltages[i] = 0.001f *
              long.Parse(s, CultureInfo.InvariantCulture);
          } catch {
            voltages[i] = null;
          }
        }

        for (int i = 0; i < temperatures.Length; i++) {
          string s = ReadFirstLine(temperatureStreams[i]);
          try {
            temperatures[i] = 0.001f *
              long.Parse(s, CultureInfo.InvariantCulture);
          } catch {
            temperatures[i] = null;
          }
        }

        for (int i = 0; i < fans.Length; i++) {
          string s = ReadFirstLine(fanStreams[i]);
          try {
            fans[i] = long.Parse(s, CultureInfo.InvariantCulture);
          } catch {
            fans[i] = null;
          }
        }
      }

      public void Close() {
        foreach (FileStream stream in voltageStreams)
          stream.Close();
        foreach (FileStream stream in temperatureStreams)
          stream.Close();
        foreach (FileStream stream in fanStreams)
          stream.Close();
      }
    }
  }
}
