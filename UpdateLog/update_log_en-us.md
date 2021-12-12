**[简体中文](./update_log.md) | [繁體中文](./update_log_zh-tw.md) | English**<br>

# TrafficMonitor Update log
## V1.82 (2021/12/12)

* Added a new plug-in system, you can display more customized content in the taskbar window and the main window through the plug-in.

* Added the "Task Manager" command to the taskbar window context menu.
* Added the "Apply" button to the option settings dialog.
* Added weekly view in historical traffic statistics.
* Added the setting of task bar item spacing.
* Fixed the problem that the taskbar window context menu cannot pop up in the dark mode of Windows 11.
* Fixed the problem that the taskbar window could not be displayed when the taskbar was restored to the Windows 10 style using software such as StartAllBack under Windows 11.

## V1.81 (2021/07/27)

* Fixed some crashes since version 1.80.
* Added the support of custom sorting of the items in the taskbar window.
* When the number of items displayed in the taskbar window is odd, the last item is arranged vertically to save space on the taskbar.
* Added the function of monitoring the hard disk usage.
* Add some settings in the right-click menu to the option settings dialog.
* Added the support for multiple monitors of the main window. You can move the main window to other monitors without checking "Allow out of screen boundaries".

## V1.80.3 (2021/05/29)

* Fixed the problem that the CPU temperature cannot be displayed
* Added the function to select the temperature of specified CPU core to monitor.
* Fixed a problem that caused the program crashed.

## V1.80.2 (2021/05/22)

* Fixed several issues that cause the program crashed.
* Use LibreHardwareMonitor to obtain temperature information.
* Added the function to select the temperature of which hard drive to monitor.
* Added the function to select the hardware to monitor.
* Fix the problem that the LibreHardwareMonitorLib.sys file is not released when the program exits.

## V1.80.1 (2021/05/17)

* Fixed the problem that the temperature of AMD GPU could not be obtained.
* Fixed the problem that the program crashes when automatically switching to the light mode color preset.
* Fixed the problem that the temperature of the 11th generation Intel processor cannot be obtained.
* Fixed the problem that the auto-start setting does not work.

## V1.80 (2021/05/15)

* Added the function of monitoring the temperature of CPU, GPU, hard disk and main board.
* Added the function of monitoring the GPU usage.
* Fixed the problem that the historical traffic statistics is not correct caused by the data overflowed.
* Fixed the problem that the main window position was incorrect after the screen resolution was changed.
* Fixed the problem that the size of the interface elements of the main window and taskbar window would not change after the system DPI was changed.
* Added the ability to freely specify display items in the taskbar window.
* Added xml format of the main window skin, supporting temperature display.
* Fixed the problem that the sub-windows in the option settings cannot be scrolled by the touch screen when using a touch screen device.
* Put the processing of checking for updates into a background thread to solve the problem that the program does not respond for a long time when checking for update.
* Fix the problem that the program is abnormal after pressing Alt+F4 in the taskbar window.
* Added support for net speeds above 4GB per second.
* Added the option to set the update source as Gitee to solve the problem that the program cannot be updated sometimes due to the inaccessibility of GitHub in mainland China.
* Added setting of memory display mode.
* Fix the problem that the currently monitored network connection is closed or disabled, the previously monitored network connection cannot be automatically selected after restarted.
* Auto run at boot is realized by Task Scheduler to solve the problem that the auto run at boot dose not work sometimes.
* Fix some crashes.
* Improvements in other details.

## V1.79.1 (2020/08/05)

* Fix the problem of registry handle leak.
* Fixed the problem that CPU and memory usage are not counted when the main window and taskbar window are not displayed.

## V1.79 (2020/07/30)

* Add the function of automatically adapts to the Windows 10 dark/light theme of the taskbar window color.
* Add the function of automatically adapts to the Windows 10 dark/light theme of the notification area icon.
* Add the option of CPU usage acquisition method. Fix the problem that the CPU usage may always 0 of some users.
* Remove the option of "Transparent color" in "Options"-"Taskbar Window Settings", and add the option of "Background Color Transparent".
* Allows only CPU and memory usage displayed in the taskbar window without the net speed.
* Fixes an problem that the settings may not be saved after the shutdown.
* Add the setting of "the first day of the week" in the "Historical Traffic Statistics"-"Calendar View".
* Add the function of displaying history traffic by day, month, quarter and year in the "Historical Traffic Statistics"-"List View".
* May fixed the problem of the loss of historical traffic data in small probability.
* Fixed the problem that the right-click menu cannot pop up if the taskbar window background is set to transparent in Windows 10 light theme.
* Fixed the problem that traffic statistics does not support data exceeding 2TB.
* Added icons for menu items.
* Save the data to the AppData directory when the program's directory cannot be written.
* Add the function of setting the monitoring interval.

## V1.78 (2020/03/21)

* Add the function to open the specified application by double-clicking the main window or taskbar window.
* Add the function to display the CPU and memory utilization status bar in the taskbar window.
* Fix the inconsistence of CPU usage with the task manager in Windows 10.
* Added option to show mouse tool tips.
* Added the function to set the taskbar color according to Windows 10 light mode when the program is first started.
* Added the function of preset schemes in the taskbar settings.
* Improvements in other details.

## V1.77 (2019/05/01)
* Add the transparent color setting of the task window. Fix the problem that the text color of the taskbar window cannot be set to black when the taskbar is white (Set the transparent color in "Options" - "Taskbar Window Settings").
* Add the function to display crash information when the program crashes.
* Fix the problem that the screen will flicker when the Windows Explorer restarts if the taskbar window is displayed.
* Display monitoring information when the mouse points to the notification area icon.
* Fixed the problem that the network speed could not be displayed when using a Bluetooth network.
* Added x64 version.
* Improvements in other details.
### Release Notes:
This update solves the problem that the text is not clear in the taskbar when the taskbar is white in the latest version of Windows 10. You need to manually set the transparent color in "Options" - "Taskbar Window Settings" - "Transparent Color". It is an opaque effect when the transparent color is not black and is different from the background color, and is transparent if it is the same as the background color. In the Windows 10 white taskbar, it is recommended to set as the following picture:
![白色任务栏截图](https://user-images.githubusercontent.com/30562462/57004858-36b55300-6c05-11e9-89d8-9911dc99f09c.PNG)
## V1.76 (2018/11/11)
* Fix the problem that the resolution changes may cause the program crash.
* Add the options for the saving location of the configuration files.
* Fix the problem that the vertical position of the taskbar window was incorrect when the resolution changed.
* Add the display of today’s upload and download traffic. Add the upload and download traffic statics in the history traffic static dialog.
* Improvements in other details.
