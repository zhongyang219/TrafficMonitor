# Inno Setup Installer for TrafficMonitor

## Overview
This repository contains an **Inno Setup script** that creates an installer for the **TrafficMonitor** application. The installer will allow users to easily install TrafficMonitor on their Windows machines.

### Features:
- Creates a simple installer to package the TrafficMonitor app.
- Allows users to choose the installation directory.
- Optionally creates shortcuts on the desktop and in the Start menu.
- Supports automatic launching after installation.

## Motivation
The TrafficMonitor app is a useful tool for monitoring network traffic, but users may find it difficult to install and configure manually. With this Inno Setup script, we can simplify the installation process, making it easier for users to get started with the app. The installer will handle the necessary files and configuration automatically, reducing setup time and user errors.

## How It Works
The Inno Setup script will perform the following steps:

1. **File Installation**:
   - The app’s executables, libraries, and configuration files will be copied to the specified installation directory (default is `C:\Program Files\TrafficMonitor`).
   
2. **Shortcuts**:
   - A shortcut to TrafficMonitor will be created on the desktop and in the Start menu.

3. **Post-installation Launch**:
   - After installation, TrafficMonitor will be automatically launched unless the user opts out during the installation process.

4. **Uninstallation**:
   - The installer also supports clean uninstallation through the "Add/Remove Programs" menu in Windows.
  
## How to Use the Inno Setup Script

1. **Install Inno Setup**:
   - Download and install [Inno Setup](http://www.jrsoftware.org/isinfo.php).

2. **Prepare Files**:
   - Ensure all necessary files for TrafficMonitor (e.g., `TrafficMonitor.exe`, configuration files, etc.) are in the correct directories on your computer.

3. **Modify the Script**:
   - Update the script with the correct file paths for your project. For example, change the paths in the `[Files]` section to point to the correct locations of your executables and resources.
   
4. **Compile the Script**:
   - Open the Inno Setup Compiler, load the `.iss` script, and click "Compile" to generate the installer.

5. **Distribute the Installer**:
   - Once the installer is created, distribute the `.exe` file. Users can simply run the installer to install TrafficMonitor.

6. **Uninstall**:
   - If users want to uninstall TrafficMonitor, they can do so from the "Add or Remove Programs" menu in Windows, and the installer will remove all the installed files.


## Conclusion

This Inno Setup script makes it easy to create an installer for the TrafficMonitor app. By following the steps above, you can generate a customized installer that simplifies the installation process for your users. Feel free to modify the script to meet your needs, and don't hesitate to contribute improvements or report issues!

If you have any questions, feel free to open an issue on GitHub, and I'll be happy to help.

Happy coding!