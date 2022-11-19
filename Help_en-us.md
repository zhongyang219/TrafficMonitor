**[简体中文](./Help.md) | English**

# TrafficMonitor Frequently Asked Questions
This is the page about the Frequently Asked Questions of TrafficMonitor. If you want to view the detailed introduction of the functions and usage of TrafficMonitor, please [click here](https://github.com/zhongyang219/TrafficMonitor/wiki) to go the the TraffinMonitor Wiki page (Only Chinese Available).

### "Taskbar window appears to the left of the taskbar" dose not work in Windows 11 when using version 1.84

In version 1.84, if the taskbar window is aligned to the left, the taskbar window will always appear on the right because it overlaps the original taskbar button when shown on the left.
The program detects taskbar alignment by reading the key value of "TaskbarAl" in the registry "\HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced". But if you never change the alignment settings of the taskbar, this key is not exist and TrafficMonitor mistakenly thinks the taskbar is left aligned. This is a bug. The solution is:
Click the right mouse button in the taskbar, select Taskbar Settings, and change the alignment method of the taskbar once. After generating the above key value, for example, change "center" to "Align left", and then change it back to "center".

### How to show the CPU and memory usage?

Right click the main window and check "Show More Info". If you also need to display the CPU and memory usage in the taskbar window, right click the taskbar window and check "CPU and Memory Usage" in the "Display Settings<img src="./Screenshots/images/item.png" style="zoom: 80%;" />" sub menu.

### How do I change the text such as "CPU" and "MEM" in the taskbar

The label text in the taskbar window can be customized. Right click on the taskbar window, select "Options", click "Taskbar Windows Settings", click "Display Text settings" button, the text on the right side of the item can be modified by double click. After modified, click "OK" button twice.

<img src="Screenshots/images/image-20221119093547649.png" alt="image-20221119093547649" style="zoom:80%;" />

Because the text is customizable, it does not automatically change when you switch the language. After you swatch the language, you can click "Restore default" button in the "Display Text settings" dialog box.

### The net speed is always displayed as 0KB

This may caused by the switch of the network you are using on your computer.

Click "Refresh connection list" under "Select network connections" in the context menu may solve this problem.

<img src="Screenshots/images/image-20221119094324759.png" alt="image-20221119094324759" style="zoom:80%;" />

If this problem still exists, please try to select the network connection you want to monitored in the "Select network connections" submenu instead of select "Auto select".

If this problem still exists, please try to click the "Select the connection to monitor" button in "Options Settings" > "General Settings" > "Advanced", check the network you want to monitored, uncheck other items, and click "OK" button twice.

<img src="Screenshots/images/image-20221119094501312.png" alt="image-20221119094501312" style="zoom:80%;" />

### How do I set the color of each item in the taskbar window individually?

Select "Options" In the right-click menu, switch to "Taskbar Window Settings" tab, check "Specify colors for each item", and then click the color block on the right side of "Text Color " to pop up the dialog box for taskbar window color settings. 

If you do not check "Specify colors for each item", you can only set the uniform color for the text.

### "Auto run when Windows start" dose not work
Starting from version 1.80, the version with temperature monitoring and the version without temperature monitoring have adopted different methods to realize "auto run when Windows start".

* Versions without temperature monitoring and versions before 1.80:

  The auto run function of the versions without temperature monitoring and versions before 1.80 is achieved by creating the "TrafficMonitor" key in the registry path of "Computer\HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run".
  If you encounter the problem of auto run dose not work, please check that the registry exists, and then check the program path is correct. If you move the location of the program, the auto run will be invalid because of the program location being invalid. In this case, you only need to uncheck "Auto run when Windows start" in the option settings, and then check it on.<br>

  Note that some third-party security software may prevent TrafficMonitor from booting up automatically. Please try to allow TrafficMonitor to boot up automatically in the security software.

  If you set the program to run as an administrator, the auto run function will also not work. Please try to remove running as an administrator.<br>

* Version with temperature monitoring:

  The version that includes temperature monitoring implements auto run function by creating a task plan. The task scheduler can be opened through `Control Panel\System and Security\Administrative Tools`.  As shown below:

  <img src="./Screenshots/images/image3.jpg"/>

  If you encounter that situation that the program cannot be started automatically after booting, please go to "Task Scheduler" to check whether the scheduled task of TrafficMonitor is created normally and the path of the exe file is correct.

A common reason why the "Auto run when Windows start" dose not work is that you may have moved the location of the TrafficMonitor main program. If you have set up TrafficMonitor to run automatically at startup, but you move TrafficMonitor to another location, then the auto run at startup will definitely fail. You need to open "Option Settings"-"General Settings" of TrafficMonitor, if "Auto run when Windows start" is checked, remove the check, then open "Option Settings"-"General Settings" again, and check again "Auto run when Windows start" will be fine.

It should be noted that if you use the version without temperature monitoring to create a auto-start item in the registry, and then use the version that with temperature monitoring to turn on the "Auto run when Windows start" function, it will automatically delete the auto-start item in the registry, and then create a auto-start item in the task plan. vice versa.

### The program pops up the "Unable to Save Settings" message box.
If you encountered this problem, that means the program does not have permission to write data to its directory, causing the settings data cannot be saved. Try to move the program to another folder that has write permissions will save this problem. <br>
You can also change the save path of the configuration and data file to the C:\Users\\<username\>\AppData\Roaming\TrafficMonitor directory by the following steps. <br>
* Exit TrafficMonitor and restart TrafficMonitor as an administrator.
* Select "Options" in the right-click menu, switch to the "General Settings" tab, and select "Save to AppData Directory" in "Configuration and data files".

If it still pops up the "Unable to Save Settings" message box, open the directory where the application is located, open the `global_cfg.ini` file. If it doesn't exist, create a new one, adding the following:

```
[config]
portable_mode = true
```

If the file cannot be created, you can create the file in a different location, such as the desktop, and then move to the directory where the program is located.

If the `global_cfg.ini` file already exists, change the value of `portable_mode` to `true` and save, then restart TrafficMonitor.

If `global_cfg.ini` does not have write permission, you can try copying the file to the desktop, and then copying it back to the original path to overwrite the original file after modified.

After these steps, this problem should not theoretically occur. If this problem still occurs, try to delete the file C:\Users\\<username\>\AppData\Roaming\TrafficMonitor\config.ini. This file will be regenerated after it is deleted. 

In version 1.79 and later, if the directory where the program located is not writable, the configuration and data files will be automatically saved to the "C:\Users\\<username\>\AppData\Roaming\TrafficMonitor" directory. At this time, "Options" -“General Settings”-“Save to the program directory” in “Configuration and data files” will not be available.

### The configuration is lost after next start up

If you do not see the "Unable to Save Settings" message box, that means the configuration file can be saved normally. The cause of this problem may be that you have multiple TrafficMonitor application files on your computer and the configuration files are saved in the application directory.

For example, you have open "D:\software\TrafficMonitor\TrafficMonitor.exe", and modified the configuration, but next time you start your computer, "D:\software1\TrafficMonitor\TrafficMonitor.exe" is started, while the configuration files you modified before is saved in "D:\software\TrafficMonitor\\".

The method to solve this problem is:

1. Change the save location of the configuration files to AppData directory in "Option Settings" > "General Settings" > "Configuration and data file".
2. Delete the TrafficMonitor application files you don't need on your computer, start TrafficMonitor, open "Option Settings" > "General Settings", click "Reset autorun" button. 

### The suspension window can only be displayed on the main monitor when multiple monitors are present.
By default, the suspension window cannot be moved out of the screen boundaries. If you need to move the suspension window to other monitors, please open "Options" > "Main Window Settings" by right click the suspension window, check "Allow Out of Screen Boundaries", and click "OK" button. At this time the suspension window is no longer limited to the screen, it can be also move to any monitors. 

If you remove the extra monitor, then the suspension window may appear outside the screen area to cause the invisible. At this time, you only need to right click the the icon in the notification area, select "Options", uncheck "Allow Out of Screen Boundaries" in "Main Window Settings", the suspension window will appear in the screen area again.

### The net speed value is not fully displayed.
Because the width of each character in different fonts is not the same, in some cases, it does appear the problem of the net speed value is not fully displayed. If this problem occurs, open "Options"-"Taskbar Window Settings", and select a larger value in the "Number of digits" drop down list.
### How to cancel after setting the mouse penetrate?
Right click the TrafficMonitor icon in the notification area, uncheck the "Mouse Penetrate".<br>
After the mouse penetrate is set, the suspension window will not be able to respond to any mouse messages, or pop-up right-click menu. But the menu can be poped up by right click the icon in notification area. The right-click menu of the main window and which of the notification area icon is exactly the same.<br>
In addition, even if you have previously set the hidden notify icon, when the "mouse penetrate" is set, the notify icon will also automatically appear to prevent the right-click menu cannot be displayed. <br>
Note: The notification area icon will be automatically displayed in the following situations: <br>
* After the "Mouse Penetrate" is set;
* Hide the main window without displaying the taskbar window;
* Close the taskbar window when the main window is hidden;
* Close the taskbar window when the "Mouse Penetrate" is set.
### Problems with the taskbar window color in Windows 10 white taskbar theme
When using the white taskbar theme, you can click the "Preset" button in the "Taskbar Window Settings" and select "Light Mode" to set the taskbar color in light mode. As the picture shows:

<img src="./Screenshots/images/image2.jpg" style="zoom:80%;" />

At the same time, you may also check "Auto adapt to Windows 10 dark/light theme", the program will automatically switch the color preset when Windows 10 dark/light theme is changed. You can click the "Auto Adapt settings" button to configure which color preset to use for dark and light themes.

### The taskbar windows cannot be displayed when the HDR is turned on in Windows 10

Some users have reported that turning on the HDR function in Windows 10 will cause the taskbar window to fail to display. If you encounter this problem, you can try turning off the "Background Transparent" option in ["Option Settings" - "Taskbar Window Settings"](https://github.com/zhongyang219/TrafficMonitor/wiki/选项设置#任务栏窗口设置).

### About the temperature monitoring of TrafficMonitor

Due to some problems occurred in some computers caused by the temperature monitoring function, the temperature monitoring function is turned off by default. If you want to use the temperature monitoring function of TrafficMonitor, please go to ["Option Settings"-"General Settings"-"Hardware Monitoring"](https://github.com/zhongyang219/TrafficMonitor/wiki/选项设置#硬件监控) to enable it. After it is turned on, temperature-related items will appear under the "Display Settings" submenu in the taskbar right-click menu.

The temperature monitoring function of TrafficMonitor relies on a third-party open source library [LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor). If you encounter the problem that the hardware temperature cannot be displayed, or the displayed temperature is incorrect, then  please download LibreHardwareMonitor first, and check whether LibreHardwareMonitor can display the corresponding temperature normally.

If LibreHardwareMonitor cannot display the temperature of the corresponding hardware, then I cannot solve this problem. You can report your problem to the author of LibreHardwareMonitor.

If LibreHardwareMonitor can display the temperature of the corresponding hardware normally, please report this problem to me, and provide a screenshot of LibreHardwareMonitor so that I can investigate your problem. 

**Note: The hardware monitoring function (including temperature monitoring and GPU usage monitoring) may have some problems, which may cause more CPU and memory usage. According to feedback from some users, turning on the temperature function will cause problems such as program crashes and system crashes. Please decide to turn on the hardware monitoring function after you are aware of the above risks. Otherwise, please do not use the hardware monitoring function.**

## The program prompts that "MSVC\*.dll" or "mfc\*.dll" cannot be found when the program starts

Click the link below to download and install the Microsoft Visual C++ runtime environment.

[Latest supported Visual C++ Redistributable downloads | Microsoft Docs](https://docs.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)

<br><br>

>If you encounter the problem of program crash, please turn off all the items in "Options" - "General Settings" - "Hardware Monitoring" after restarting the program, because based on the DMP file provided by the user, most crash problems are related to the hardware monitoring function. I cannot solve the crash problem related to the hardware monitoring function, please do not send me email feedback. If you have excluded the hardware monitoring problem, but the crash problem still exists, please go to "Options" - "General Settings" - "Advanced" - "Plug-in Management" to disable all plugins, and then restart the program. If you have excluded the plugin problem, but the crash problem still exists, please send me the email according to the prompts in the crash window.
>
>If you have encountered any other problems, you can also click "Contact Author" in the "About" dialog box, or directly [click here](mailto:zhongyang219@hotmail.com) to send me an email.<br>
>Please describe the problems you have encountered in detail, the error prompts, what operations you have tried, it is better to attach the screenshots and configuration files ("Options"-"General Settings"-"Open configuration file path"). <br>
>Note: Please confirm the email address that you used before sending the email. If your email address is form like "Outlook_\<random string\>@outlook.com", then such email address is unable to reply.
>This may be due to the fact that you are using a third-party e-mail address as the Microsoft account to log on to Windows. If this is the case, be sure to enclose the correct email address in the message.
