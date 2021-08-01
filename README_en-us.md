**[简体中文](./README.md) | [繁體中文](./README_zh-tw.md) | English**<br>
[![Badge](https://img.shields.io/badge/link-996.icu-%23FF4D5B.svg?style=flat-square)](https://996.icu/#/en_US)
[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg?style=flat-square)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
[![GitHub Workflow Status](https://img.shields.io/github/workflow/status/zhongyang219/TrafficMonitor/Release%20CI?label=Release%20CI&logo=github&style=flat-square)](https://github.com/zhongyang219/TrafficMonitor/actions?query=workflow:"Release+CI")
[![GitHub release](https://img.shields.io/github/release/zhongyang219/TrafficMonitor.svg?style=flat-square)](https://github.com/zhongyang219/TrafficMonitor/releases/latest)

# TrafficMonitor Introduction
TrafficMonitor is a network monitoring suspension window software in Windows platform. It can display the current network speed, CPU and memory usage. It also support the functions of display in the taskbar, change skin and historical traffic statistics. 

# Related Links

Please [click here](https://github.com/zhongyang219/TrafficMonitor/releases/latest) to download the latest version of TrafficMonitor.<br>
Alternate link: Download from [Baidu Netdisk](https://pan.baidu.com/s/15PMt7s-ASpyDwtS__4cUhg). Access code: `ou0m`<br>
If you encounter any problems, please [click here](./Help_en-us.md).<br>

You can also [click here](https://github.com/zhongyang219/TrafficMonitor/actions?query=workflow:"Release+CI") to download the pre-release build version of TrafficMonitor.

Starting from version 1.80,  the temperature monitoring function has been added. If you do not need the temperature monitoring function and you encounter problems with the version 1.80 or higher, it is recommended to download the version without temperature monitoring. (Find the version that contains `without_temperature` in the file name on the Release page.)

# Main Features
* Displays the current network transfer speed, CPU and memory usage.<br>
* Support automatic or manual selection of network connections if there are multiple network adapters.<br>
* Display the network connection details.<br>
* Support the window displayed in the taskbar.<br>
* Support for changing skin and customizing skin.<br>
* Historical traffic statistics.<br>
# Instructions for use

**[Click here](https://github.com/zhongyang219/TrafficMonitor/wiki) to go to the Wiki page to view detailed documentation on TrafficMonitor (Only Chinese Available).**

# Screen Shots

Main Suspension Window:<br>
![](./Screenshots/en_us/main1.png)<br>
Right-Click Menu:<br>
![](./Screenshots/en_us/main.png)<br>
Taskbar Window<br>
![](./Screenshots/en_us/taskbar.png)<br>
Colorful Skins:<br>
![](./Screenshots/skins.PNG)<br>
Change Skins:<br>
![](./Screenshots/en_us/selecte_skin.png)<br>
Options:<br>
<img src="./Screenshots/en_us/option.jpg" style="zoom:80%;" /><br>

# How to Use

After the program is started, a floating window showing the network speed will be displayed on the screen. Click the right mouse button on the floating window to pop up the right-click menu.

TrafficMonitor supports displaying information to the taskbar. But TrafficMonitor only displays the main window (floating window) by default. If you want to embed it in the taskbar display, please select the "Show taskbar window" command in the right-click menu.

Taskbar window supports customization of display items. By default, only the network speed is displayed. If you need to display the CPU and memory usage, please check the items that need to be displayed under the "Display Settings" submenu in the taskbar right-click menu, as shown in the figure below:

<img src="./Screenshots/en_us/taskbar_item_settings.png" style="zoom:80%;" />

# About the hardware monitoring function

Starting from version 1.80,  the hardware monitoring functions (including temperature monitoring and graphics card usage monitoring) has been added in TrafficMonitor. It relied on the  open source library [LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor). If you encounter problems when using the temperature monitoring function, please [click here](./Help_en-us.md#13-about-the-temperature-monitoring-of-trafficmonitor). 

It should be noted that the temperature monitoring function is turned off by default. If you want to use the temperature monitoring function of TrafficMonitor, please go to ["Option Settings"-"General Settings"-"Hardware Monitoring"](https://github.com/zhongyang219/TrafficMonitor/wiki/选项设置#硬件监控) to enable it.

**Note: The hardware monitoring function (including temperature monitoring and GPU usage monitoring) may have some problems, which may cause more CPU and memory usage. According to feedback from some users, turning on the temperature function will cause problems such as program crashes and system crashes. Please decide to turn on the hardware monitoring function after you are aware of the above risks. Otherwise, please do not use the hardware monitoring function.**

# Update log

**[Click here to view the update log.](./UpdateLog/update_log_en-us.md)**
