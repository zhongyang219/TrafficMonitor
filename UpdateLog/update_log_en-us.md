**[简体中文](./update_log.md) | [繁體中文](./update_log_zh-tw.md) | English**<br>

# TrafficMonitor Update log
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
