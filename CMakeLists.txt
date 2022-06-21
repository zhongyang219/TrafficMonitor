# Copyright (c) <2019> <copyright zhongyang219>

# "Anti 996" License Version 1.0 (Draft)

# Permission is hereby granted to any individual or legal entity
# obtaining a copy of this licensed work (including the source code,
# documentation and/or related items, hereinafter collectively referred
# to as the "licensed work"), free of charge, to deal with the licensed
# work for any purpose, including without limitation, the rights to use,
# reproduce, modify, prepare derivative works of, distribute, publish
# and sublicense the licensed work, subject to the following conditions:

# 1. The individual or the legal entity must conspicuously display,
# without modification, this License and the notice on each redistributed
# or derivative copy of the Licensed Work.

# 2. The individual or the legal entity must strictly comply with all
# applicable laws, regulations, rules and standards of the jurisdiction
# relating to labor and employment where the individual is physically
# located or where the individual was born or naturalized; or where the
# legal entity is registered or is operating (whichever is stricter). In
# case that the jurisdiction has no such laws, regulations, rules and
# standards or its laws, regulations, rules and standards are
# unenforceable, the individual or the legal entity are required to
# comply with Core International Labor Standards.

# 3. The individual or the legal entity shall not induce, suggest or force
# its employee(s), whether full-time or part-time, or its independent
# contractor(s), in any methods, to agree in oral or written form, to
# directly or indirectly restrict, weaken or relinquish his or her
# rights or remedies under such laws, regulations, rules and standards
# relating to labor and employment as mentioned above, no matter whether
# such written or oral agreements are enforceable under the laws of the
# said jurisdiction, nor shall such individual or the legal entity
# limit, in any methods, the rights of its employee(s) or independent
# contractor(s) from reporting or complaining to the copyright holder or
# relevant authorities monitoring the compliance of the license about
# its violation(s) of the said license.

# THE LICENSED WORK IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN ANY WAY CONNECTION WITH THE
# LICENSED WORK OR THE USE OR OTHER DEALINGS IN THE LICENSED WORK.

cmake_minimum_required(VERSION 3.11)

project(TrafficMonitor)

## options
option(ENABLED_LITE "debug switch" OFF)

## lite version
if(ENABLED_LITE)
add_definitions(-DWITHOUT_TEMPERATURE)
endif()

## enabled cxx 14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

## enable mfc
ADD_DEFINITIONS(-D_AFXDLL)
SET(CMAKE_MFC_FLAG 2)

## add hardwaremonitorapi plugin
add_subdirectory(OpenHardwareMonitorApi)

## add trafficmonitor sources
file(GLOB traffic_SRC ${CMAKE_CURRENT_SOURCE_DIR}/include/PluginInterface.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/AboutDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/AdapterCommon.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/AppAlreadyRuningDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/auto_start_helper.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/BaseDialog.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CalendarHelper.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CAutoAdaptSettingsDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CMFCColorDialogEx.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/ColorSettingListCtrl.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/ComboBox2.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CPUUsage.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/crashtool.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CTabCtrlEx.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CVariant.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/DisplayTextSettingDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/DrawCommonEx.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/FilePathHelper.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/HighResolutionTimer.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/HistoryTrafficCalendarDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/HistoryTrafficFile.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/HistoryTrafficListDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/LinkStatic.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/ColorStatic.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/Common.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CommonData.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CSkinPreviewView.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/DonateDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/DrawCommon.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/GeneralSettingsDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/HistoryTrafficDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/IconSelectDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/IniHelper.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/HistoryTrafficListCtrl.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/ListCtrlEx.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/MainWndColorDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/MainWndSettingsDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/MessageDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/NetworkInfoDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/OptionsDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/PictureStatic.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/PluginInfoDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/PluginManager.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/PluginManagerDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/Resource.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/SelectConnectionsDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/SetItemOrderDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/SimpleXML.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/SkinDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/SkinFile.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/SpinEdit.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/StaticEx.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/stdafx.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TabDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/targetver.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TaskbarColorDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TaskbarDefaultStyle.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TaskBarDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TaskbarItemOrderHelper.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TaskBarSettingsDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/Test.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TinyXml2Helper.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/tinyxml2/tinyxml2.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TrafficMonitor.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TrafficMonitorDlg.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/UpdateHelper.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/WIC.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/WindowsSettingHelper.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/WinVersionHelper.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/AboutDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/AdapterCommon.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/AppAlreadyRuningDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/auto_start_helper.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/BaseDialog.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CalendarHelper.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CAutoAdaptSettingsDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CMFCColorDialogEx.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/ColorSettingListCtrl.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/ComboBox2.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CommonData.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CPUUsage.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/crashtool.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CTabCtrlEx.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CVariant.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/DisplayTextSettingDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/DrawCommonEx.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/FilePathHelper.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/HistoryTrafficCalendarDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/HistoryTrafficFile.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/HistoryTrafficListDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/LinkStatic.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/ColorStatic.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/Common.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/CSkinPreviewView.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/DonateDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/DrawCommon.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/GeneralSettingsDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/HistoryTrafficDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/IconSelectDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/IniHelper.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/HistoryTrafficListCtrl.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/ListCtrlEx.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/MainWndColorDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/MainWndSettingsDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/MessageDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/NetworkInfoDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/OptionsDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/PictureStatic.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/PluginInfoDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/PluginManager.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/PluginManagerDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/SelectConnectionsDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/SetItemOrderDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/SimpleXML.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/SkinDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/SkinFile.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/SpinEdit.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/StaticEx.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/stdafx.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TabDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TaskbarColorDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TaskbarDefaultStyle.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TaskBarDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TaskbarItemOrderHelper.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TaskBarSettingsDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/Test.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TinyXml2Helper.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/tinyxml2/tinyxml2.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TrafficMonitor.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TrafficMonitorDlg.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/UpdateHelper.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/WIC.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/WindowsSettingHelper.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/WinVersionHelper.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/TrafficMonitor.rc
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/license.bin
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/TrafficMonitor.rc2
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/about_background.bmp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/about_background_hd.bmp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/bitmap3.bmp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/donate.bmp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/donate_wechart.bmp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/close.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/connection.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/exit.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/function.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/help.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/info.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/item.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/lock.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/main_window.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/more.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/mouse.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/notify.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/pin.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/plugins.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/setting.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/skn.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/statistics.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/taskbar_window.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/menu_icon/task_manager.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/notifyicon.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/notifyicon2.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/notifyicon3.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/notifyicon4.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/notifyicon5.ico
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/notify_preview.bmp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/notify_preview_light.bmp
                    ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/res/TrafficMonitor.ico)

## main executor 
add_executable(${PROJECT_NAME} ${traffic_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

## enabled window executor
IF(MSVC)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
ENDIF(MSVC)

#support unicode
target_compile_definitions(${PROJECT_NAME} PRIVATE _AFXDLL _UNICODE UNICODE)

## link hardware plugin
target_link_libraries(${PROJECT_NAME} PRIVATE OpenHardwareMonitorApi)

## copy skins resources  vcxproj run
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/skins DESTINATION ${CMAKE_BINARY_DIR})


#copy resources binary dir
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/skins DESTINATION "${CMAKE_BINARY_DIR}/Debug")
elseif(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/skins DESTINATION "${CMAKE_BINARY_DIR}/Release")
else()
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/skins DESTINATION "${CMAKE_BINARY_DIR}/Debug")
endif()

string(TIMESTAMP COMPILE_TIME %Y%m%d_%H%M%S)
message(STATUS "==>> ${COMPILE_TIME}")
add_custom_command(TARGET ${PROJECT_NAME}
            PRE_BUILD
            COMMAND echo ${COMPILE_TIME} >> ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/compile_time.txt)

# has some problem
# add_custom_command(TARGET trafficmonitor
#             PRE_BUILD
#             COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/print_compile_time.bat
#             WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/TrafficMonitor/)

# set_target_properties(trafficmonitor PROPERTIES
#     COMPILE_DEFINITIONS _AFXDLL,_UNICODE,UNICODE,_BIND_TO_CURRENT_CRT_VERSION,_BIND_TO_CURRENT_MFC_VERSION
#     LINK_FLAGS "/ENTRY:\"wWinMainCRTStartup\"")