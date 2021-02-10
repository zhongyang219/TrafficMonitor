/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

namespace OpenHardwareMonitor.Hardware.LPC {

  internal enum Chip : ushort {
    Unknown = 0,

    ATK0110 = 0x0110,

    F71858 = 0x0507,
    F71862 = 0x0601, 
    F71869 = 0x0814,
    F71869A = 0x1007,
    F71878AD = 0x1106,
    F71882 = 0x0541,
    F71889AD = 0x1005,
    F71889ED = 0x0909,
    F71889F = 0x0723,
    F71808E = 0x0901,

    IT8620E = 0x8620,
    IT8628E = 0x8628,
    IT8655E = 0x8655,
    IT8665E = 0x8665,
    IT8686E = 0x8686,
    IT8688E = 0x8688,
    IT8705F = 0x8705,
    IT8712F = 0x8712,
    IT8716F = 0x8716,
    IT8718F = 0x8718,
    IT8720F = 0x8720,
    IT8721F = 0x8721,
    IT8726F = 0x8726,
    IT8728F = 0x8728,
    IT879XE = 0x8733, // IT8792E, IT8795E
    IT8771E = 0x8771,
    IT8772E = 0x8772,    

    NCT6771F = 0xB470,
    NCT6776F = 0xC330,
    NCT610X = 0xC452,
    NCT6779D = 0xC560,
    NCT6791D = 0xC803,
    NCT6792D = 0xC911,
    NCT6792DA = 0xC913,
    NCT6793D = 0xD121,
    NCT6795D = 0xD352,
    NCT6796D = 0xD423,
    NCT6796DR = 0xD42A,
    NCT6797D = 0xD451,
    NCT6798D = 0xD42B,

    W83627DHG = 0xA020,
    W83627DHGP = 0xB070,
    W83627EHF = 0x8800,    
    W83627HF = 0x5200,
    W83627THF = 0x8280,
    W83667HG = 0xA510,
    W83667HGB = 0xB350,
    W83687THF = 0x8541
  }

  internal class ChipName {

    private ChipName() { }

    public static string GetName(Chip chip) {
      switch (chip) {
        case Chip.ATK0110: return "Asus ATK0110";

        case Chip.F71858: return "Fintek F71858";
        case Chip.F71862: return "Fintek F71862";
        case Chip.F71869: return "Fintek F71869";
        case Chip.F71878AD: return "Fintek F71878AD";
        case Chip.F71869A: return "Fintek F71869A";
        case Chip.F71882: return "Fintek F71882";
        case Chip.F71889AD: return "Fintek F71889AD";
        case Chip.F71889ED: return "Fintek F71889ED";
        case Chip.F71889F: return "Fintek F71889F";
        case Chip.F71808E: return "Fintek F71808E";

        case Chip.IT8620E: return "ITE IT8620E";
        case Chip.IT8628E: return "ITE IT8628E";
        case Chip.IT8655E: return "ITE IT8655E";
        case Chip.IT8665E: return "ITE IT8665E";
        case Chip.IT8686E: return "ITE IT8686E";
        case Chip.IT8688E: return "ITE IT8688E";
        case Chip.IT8705F: return "ITE IT8705F";
        case Chip.IT8712F: return "ITE IT8712F";
        case Chip.IT8716F: return "ITE IT8716F";
        case Chip.IT8718F: return "ITE IT8718F";        
        case Chip.IT8720F: return "ITE IT8720F";
        case Chip.IT8721F: return "ITE IT8721F";
        case Chip.IT8726F: return "ITE IT8726F";
        case Chip.IT8728F: return "ITE IT8728F";
        case Chip.IT879XE: return "ITE IT879XE";
        case Chip.IT8771E: return "ITE IT8771E";
        case Chip.IT8772E: return "ITE IT8772E";        

        case Chip.NCT610X: return "Nuvoton NCT610X";

        case Chip.NCT6771F: return "Nuvoton NCT6771F";
        case Chip.NCT6776F: return "Nuvoton NCT6776F";
        case Chip.NCT6779D: return "Nuvoton NCT6779D";
        case Chip.NCT6791D: return "Nuvoton NCT6791D";
        case Chip.NCT6792D: return "Nuvoton NCT6792D+";
        case Chip.NCT6792DA: return "Nuvoton NCT6792D-A";
        case Chip.NCT6793D: return "Nuvoton NCT6793D";
        case Chip.NCT6795D: return "Nuvoton NCT6795D";
        case Chip.NCT6796D: return "Nuvoton NCT6796D";
        case Chip.NCT6796DR: return "Nuvoton NCT6796D-R";
        case Chip.NCT6797D: return "Nuvoton NCT6797D";
        case Chip.NCT6798D: return "Nuvoton NCT6798D";

        case Chip.W83627DHG: return "Winbond W83627DHG";
        case Chip.W83627DHGP: return "Winbond W83627DHG-P";
        case Chip.W83627EHF: return "Winbond W83627EHF";
        case Chip.W83627HF: return "Winbond W83627HF";
        case Chip.W83627THF: return "Winbond W83627THF";
        case Chip.W83667HG: return "Winbond W83667HG";
        case Chip.W83667HGB: return "Winbond W83667HG-B";
        case Chip.W83687THF: return "Winbond W83687THF";

        case Chip.Unknown: return "Unkown";
        default: return "Unknown";
      }
    }
  }

}
