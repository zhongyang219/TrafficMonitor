**[简体中文](./README.md) | English**<br>
[![Badge](https://img.shields.io/badge/link-996.icu-%23FF4D5B.svg?style=flat-square)](https://996.icu/#/en_US)
[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg?style=flat-square)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
[![GitHub Workflow Status](https://img.shields.io/github/workflow/status/zhongyang219/TrafficMonitor/Release%20CI?label=Release%20CI&logo=github&style=flat-square)](https://github.com/zhongyang219/TrafficMonitor/actions?query=workflow:"Release+CI")
[![GitHub release](https://img.shields.io/github/release/zhongyang219/TrafficMonitor.svg?style=flat-square)](https://github.com/zhongyang219/TrafficMonitor/releases/latest)

# TrafficMonitor Introduction
TrafficMonitor is a network monitoring suspension window software on Windows platform. It can display the current network speed, usage of CPU. It also has the function of displaying in the taskbar, skin changing and historical traffic statistics. 

# Related Links

Please [click here](https://github.com/zhongyang219/TrafficMonitor/releases/latest) to download the latest version of TrafficMonitor. 

Alternate link: Download from [Baidu Netdisk](https://pan.baidu.com/s/15PMt7s-ASpyDwtS__4cUhg). Access code: `ou0m`  

If you encounter any problems, please [click here](./Help_en-us.md).  

You can also [click here](https://github.com/zhongyang219/TrafficMonitor/actions?query=workflow:"Release+CI") to download the pre-release build version of TrafficMonitor.

Starting from version 1.80, the temperature monitoring function has been added. If you do not need the temperature monitoring function and encounter problems with version 1.80 or higher, it is recommended to download the version which is without temperature monitoring (Lite version). (Find the version that contains `Lite` in the file name on Release page.)

TrafficMonitor relies on the Microsoft Visual C++ runtime environment. If the program prompts "Cannot find MSVC*.dll" when the program starts, please click the link below to download and install the Microsoft Visual C++ runtime environment.

[Latest supported Visual C++ Redistributable downloads | Microsoft Docs](https://docs.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)

# Version description

TrafficMonitor provides two versions, the standard version and the Lite version. The standard version includes all the functions, while the Lite version does not include hardware monitoring functions such as temperature monitoring, GPU usage, and hard disk usage. The standard version requires administrator to run, while the Lite version does not. 

If there is no need to monitor the temperature and other hardware information, it is recommended to use the Lite version.

The functions comparison of the two versions is as the following.

| Functions                                              | Standard Version | Lite Version |
| ------------------------------------------------------ | ---------------- | ------------ |
| Net speed monitoring                                   | ✔                | ✔            |
| CPU and memory usage monitoring                        | ✔                | ✔            |
| CPU, GPU, hard disk, main board temperature monitoring | ✔                | ❌            |
| GPU usage monitoring                                   | ✔                | ❌            |
| Hard disk usage monitoring                             | ✔                | ❌            |
| Network connection details                             | ✔                | ✔            |
| Plug-in system                                         | ✔                | ✔            |
| Change the skin of the main window                     | ✔                | ✔            |
| Administrator rights required                          | Yes              | No           |

# Main Features

* Displays the current network transfer speed, usage of CPU and memory.
* Support automatic or manual selection of network connections if there are multiple network adapters.
* Display the network connection details.
* Support the window displayed in the taskbar.
* Support for changing skin and customizing skin.
* Historical traffic statistics.
* Hardware information monitoring
* Plug-in system

# Instructions for use

**[Click here](https://github.com/zhongyang219/TrafficMonitor/wiki) to go to the Wiki page to view detailed documentation on TrafficMonitor (Only Chinese Available).**

# Screen Shots

Main Suspension Window:  
![](./Screenshots/en_us/main1.png)  
Right-Click Menu:  
![](./Screenshots/en_us/main.png)  
Taskbar Window  
![](./Screenshots/en_us/taskbar.png)  
Colorful Skins:  
![](./Screenshots/skins.PNG)  
Change Skins:  
![](./Screenshots/en_us/selecte_skin.png)  
Options:  
<img src="./Screenshots/en_us/option.jpg" style="zoom:80%;" />  

# How to Use

After the program started, a floating window showing the network speed will be displayed on the screen. Click the right mouse button on the floating window to pop up the right-click menu.

TrafficMonitor supports displaying information on the taskbar. But TrafficMonitor only displays the main window (floating window) by default. If you want to embed it into the taskbar, please select the "Show taskbar window" command in the right-click menu.

Taskbar window supports customization of display items. By default, only the network speed is displayed. If you need to display the CPU and memory usage, please check the items that need to be displayed under the "Display Settings" submenu in the taskbar right-click menu, as shown in the figure below:

<img src="./Screenshots/en_us/taskbar_item_settings.png" style="zoom:80%;" />

# Plug-in system

The plug-in system has been added since version 1.82. The plug-in dll must be placed in the "plugins" directory of the same level directory of "TrafficMonitor.exe". After the program is started, the plug-in will be loaded automatically. You can view and manage the loaded plugins in the context menu "More Functions"-"Plugin Management".

For instructions on how to develop the plugin-in of TrafficMonitor, please see [Plugin Development Guide · zhongyang219/TrafficMonitor Wiki (github.com)](https://github.com/zhongyang219/TrafficMonitor/wiki/Plugin-Development-Guide).

To download the TrafficMonitor plug-in, please [click here](https://github.com/zhongyang219/TrafficMonitorPlugins/blob/main/download/plugin_download.md).

# About the hardware monitoring function

Starting from version 1.80, the hardware monitoring functions (including temperature monitoring and graphics card usage monitoring) has been added in TrafficMonitor. It relies on the open source library [LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor). If you encounter problems during using the temperature monitoring function, please [click here](./Help_en-us.md#13-about-the-temperature-monitoring-of-trafficmonitor). 

It should be noted that the temperature monitoring function is turned off by default. If you want to use the temperature monitoring function of TrafficMonitor, please go to ["Option Settings"-"General Settings"-"Hardware Monitoring"](https://github.com/zhongyang219/TrafficMonitor/wiki/选项设置#硬件监控) to enable it.

**Note: The hardware monitoring function (including temperature monitoring and GPU usage monitoring) may have some problems, which may cause more CPU and memory usage. According to feedback from some users, turning on the temperature function will cause problems such as program crashes and system crashes. Please decide to turn on the hardware monitoring function after you are aware of the above risks. Otherwise, please do not use the hardware monitoring function.**

# Update log

**[Click here to view the update log.](./UpdateLog/update_log_en-us.md)**
