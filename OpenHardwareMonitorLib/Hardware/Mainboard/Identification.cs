/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2012-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

namespace OpenHardwareMonitor.Hardware.Mainboard {
  internal class Identification {

    public static Manufacturer GetManufacturer(string name) {
      switch (name) {
        case "acer":
        case "Acer":
        case "ACER":
        case "Acer, Inc.":
          return Manufacturer.Acer;
        case "AMD":
        case "AMD Corp.":
        case "AMD Corporation":
          return Manufacturer.AMD;
        case "Alienware":
          return Manufacturer.Alienware;
        case "AOpen":
        case "AOpen Inc.":
          return Manufacturer.AOpen;
        case "Apple Computer, Inc.":
        case "Apple Inc.":
          return Manufacturer.Apple;
        case "ASRock":
          return Manufacturer.ASRock;
        case "ASUS CORPORATION":
        case "ASUSTek Computer Inc.":
        case "ASUSTek Computer INC.":
        case "ASUSTeK Computer Inc.":
        case "ASUSTeK Computer INC.":
        case "ASUSTeK COMPUTER INC.":
          return Manufacturer.ASUS;
        case "Biostar":
        case "Biostar Group":
        case "BIOSTAR Group":
          return Manufacturer.Biostar;
        case "clevo":
        case "Clevo":
        case "CLEVO":
        case "CLEVO Co.":
        case "CLEVO CO.":
          return Manufacturer.Clevo;
        case "Dell Computer Corp.":
        case "Dell Computer Corporation":
        case "Dell Inc":
        case "Dell Inc.":
        case "DELL Inc.":
        case "DellInc.":
          return Manufacturer.Dell;
        case "DFI":
        case "DFI Inc.":
          return Manufacturer.DFI;
        case "ECS":
        case "ELITEGROUP":
        case "ELITEGROUP COMPUTER SYSTEM CO.,LTD.":
          return Manufacturer.ECS;
        case "EPoX COMPUTER CO., LTD":
          return Manufacturer.EPoX;
        case "EVGA":
        case "EVGA INTERNATIONAL CO.,LTD":
          return Manufacturer.EVGA;
        case "FIC":
        case "First International Computer, Inc.":
          return Manufacturer.FIC;
        case "Foxconn":
        case "FOXCONN":
          return Manufacturer.Foxconn;
        case "FUJITSU":
        case "FUJITSU SIEMENS":
        case "FUJITSU-SV":
          return Manufacturer.Fujitsu;
        case "Gateway":
        case "GATEWAY":
          return Manufacturer.Gateway;
        case "Gigabyte":
        case "GIGABYTE":
        case "Gigabyte Technology Co., Ltd.":
        case "Gigabyte Technology Co.,Ltd":
        case "Gigabyte Technology Co.,Ltd.":
        case "Gigabyte Tecohnology Co., Ltd.":
          return Manufacturer.Gigabyte;
        case "Hewleet-Packard":
        case "Hewlett-Packard":
        case "HP":
          return Manufacturer.HP;
        case "http://www.abit.com.tw/":
        case "www.abit.com.tw":
          return Manufacturer.Abit;
        case "IBM":
          return Manufacturer.IBM;
        case "Intel":
        case "INTEL":
        case "Intel Corp.":
        case "Intel Corporation":
        case "INTEL Corporation":
        case "Intel.":
          return Manufacturer.Intel;
        case "JETWAY":
          return Manufacturer.Jetway;
        case "LattePanda":
          return Manufacturer.LattePanda;
        case "Lenovo":
        case "LENOVO":
          return Manufacturer.Lenovo;
        case "Medion":
        case "MEDION":
        case "MEDIONPC":
          return Manufacturer.Medion;
        case "Microsoft Corporation":
          return Manufacturer.Microsoft;
        case "Micro Star":
        case "Micro-Star":
        case "MICRO-STAR INC.":
        case "MICRO-STAR INTERANTIONAL CO.,LTD":
        case "MICRO-STAR INTERANTONAL CO.,LTD":
        case "Micro-Star International":
        case "Micro-Star International Co., Ltd":
        case "MICRO-STAR INTERNATIONAL CO., LTD":
        case "Micro-Star International Co., Ltd.":
        case "MICRO-STAR INTERNATIONAL CO.,LTD":
        case "msi":
        case "MSI":
          return Manufacturer.MSI;
        case "NEC":
        case "NEC COMPUTERS INTERNATIONAL":
          return Manufacturer.NEC;
        case "PEGATRON CORPORATION":
        case "PEGATRON CORPORATION.":
        case "PEGATRON INC.":
          return Manufacturer.Pegatron;
        case "SAMSUNG ELECTRONICS CO., LTD.":
        case "SAMSUNG ELECTRONICS CO.,LTD":
          return Manufacturer.Samsung;
        case "SAPPHIRE":
        case "Sapphire Tech":
          return Manufacturer.Sapphire;
        case "Shuttle":
        case "Shuttle Inc":
        case "Shuttle Inc.":
          return Manufacturer.Shuttle;
        case "Sony Corporation":
          return Manufacturer.Sony;
        case "Supermicro":
          return Manufacturer.Supermicro;
        case "TOSHIBA":
          return Manufacturer.Toshiba;
        case "XFX":
          return Manufacturer.XFX;
        case "ZOTAC":
          return Manufacturer.ZOTAC;
        case "To be filled by O.E.M.":
          return Manufacturer.Unknown;
        default:
          return Manufacturer.Unknown;
      }
    }

    public static Model GetModel(string name) {
      switch (name) {
        case "880GMH/USB3":
          return Model._880GMH_USB3;
        case "ASRock AOD790GX/128M":
          return Model.AOD790GX_128M;
        case "P55 Deluxe":
          return Model.P55_Deluxe;
        case "Crosshair III Formula":
          return Model.Crosshair_III_Formula;
        case "M2N-SLI DELUXE":
          return Model.M2N_SLI_DELUXE;
        case "M4A79XTD EVO":
          return Model.M4A79XTD_EVO;
        case "P5W DH Deluxe":
          return Model.P5W_DH_Deluxe;
        case "P6T":
          return Model.P6T;
        case "P6X58D-E":
          return Model.P6X58D_E;
        case "P8P67":
          return Model.P8P67;
        case "P8P67 EVO":
          return Model.P8P67_EVO;
        case "P8P67 PRO":
          return Model.P8P67_PRO;
        case "P8P67-M PRO":
          return Model.P8P67_M_PRO;
        case "P8Z77-V":
          return Model.P8Z77_V;
        case "P9X79":
          return Model.P9X79;
        case "Rampage Extreme":
          return Model.Rampage_Extreme;
        case "Rampage II GENE":
          return Model.Rampage_II_GENE;
        case "LP BI P45-T2RS Elite":
          return Model.LP_BI_P45_T2RS_Elite;
        case "LP DK P55-T3eH9":
          return Model.LP_DK_P55_T3eH9;
        case "A890GXM-A":
          return Model.A890GXM_A;
        case "X58 SLI Classified":
          return Model.X58_SLI_Classified;
        case "965P-S3":
          return Model._965P_S3;
        case "EP45-DS3R":
          return Model.EP45_DS3R;
        case "EP45-UD3R":
          return Model.EP45_UD3R;
        case "EX58-EXTREME":
          return Model.EX58_EXTREME;
        case "EX58-UD3R":
          return Model.EX58_UD3R;
        case "G41M-Combo":
          return Model.G41M_Combo;
        case "G41MT-S2":
          return Model.G41MT_S2;
        case "G41MT-S2P":
          return Model.G41MT_S2P;
        case "GA-970A-UD3":
          return Model.GA_970A_UD3;
        case "GA-MA770T-UD3":
          return Model.GA_MA770T_UD3;
        case "GA-MA770T-UD3P":
          return Model.GA_MA770T_UD3P;
        case "GA-MA785GM-US2H":
          return Model.GA_MA785GM_US2H;
        case "GA-MA785GMT-UD2H":
          return Model.GA_MA785GMT_UD2H;
        case "GA-MA78LM-S2H":
          return Model.GA_MA78LM_S2H;
        case "GA-MA790X-UD3P":
          return Model.GA_MA790X_UD3P;
        case "H55-USB3":
          return Model.H55_USB3;
        case "H55N-USB3":
          return Model.H55N_USB3;
        case "H61M-DS2 REV 1.2":
          return Model.H61M_DS2_REV_1_2;
        case "H61M-USB3-B3 REV 2.0":
          return Model.H61M_USB3_B3_REV_2_0;
        case "H67A-UD3H-B3":
          return Model.H67A_UD3H_B3;
        case "H67A-USB3-B3":
          return Model.H67A_USB3_B3;
        case "P35-DS3":
          return Model.P35_DS3;
        case "P35-DS3L":
          return Model.P35_DS3L;
        case "P55-UD4":
          return Model.P55_UD4;
        case "P55A-UD3":
          return Model.P55A_UD3;
        case "P55M-UD4":
          return Model.P55M_UD4;
        case "P67A-UD3-B3":
          return Model.P67A_UD3_B3;
        case "P67A-UD3R-B3":
          return Model.P67A_UD3R_B3;
        case "P67A-UD4-B3":
          return Model.P67A_UD4_B3;
        case "P8Z68-V PRO":
          return Model.P8Z68_V_PRO;
        case "X38-DS5":
          return Model.X38_DS5;
        case "X570 AORUS MASTER":
          return Model.X570_AORUS_MASTER;
        case "X58A-UD3R":
          return Model.X58A_UD3R;
        case "Z68A-D3H-B3":
          return Model.Z68A_D3H_B3;
        case "Z68AP-D3":
          return Model.Z68AP_D3;
        case "Z68X-UD3H-B3":
          return Model.Z68X_UD3H_B3;
        case "Z68X-UD7-B3":
          return Model.Z68X_UD7_B3;
        case "Z390 M GAMING-CF":
          return Model.Z390_M_GAMING;
        case "Z390 AORUS ULTRA":
          return Model.Z390_AORUS_ULTRA;
        case "Z390 UD":
          return Model.Z390_UD;
        case "FH67":
          return Model.FH67;
        case "Base Board Product Name":
        case "To be filled by O.E.M.":
          return Model.Unknown;
        default:
          return Model.Unknown;
      }
    }

  }
}
