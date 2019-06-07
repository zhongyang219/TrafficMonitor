
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

#include "resource.h"
#include <string>
using std::string;
using std::wstring;
#include <vector>
using std::vector;
#include <deque>
using std::deque;
#include<iostream>
#include<fstream>
using std::ifstream;
using std::ofstream;
#include<io.h>
#include<algorithm>
#include <cmath>
#include <afxinet.h>	//用于支持使用网络相关的类


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#define MY_WM_NOTIFYICON (WM_USER+1001)

//#define CONFIG_PATH _T(".\\config.ini")
//#define CONFIG_PATHA ".\\config.ini"
//#define LOG_PATH _T(".\\error.log")
//#define HISTORY_TRAFFIC_PATH _T(".\\history_traffic.dat")
#define MAX_RESTART_CNT 10		//最大重新初始化次数

#define MAIN_TIMER 1234			//主定时器的ID
#define DELAY_TIMER 1235		//延时定时器ID
#define TASKBAR_TIMER 1236
#define CONNECTION_DETAIL_TIMER 1237

#define MAX_INSERT_TO_TASKBAR_CNT 5		//尝试嵌入任务栏的最大次数

#define VERSION L"1.77"
#define COMPILE_DATE L"2019/05/01"

#define MAX_NOTIFY_ICON 4		//可选的通知区图标数量

//背景图片的文件名
#define BACKGROUND_IMAGE_S L"\\background.bmp"
#define BACKGROUND_IMAGE_L L"\\background_l.bmp"
#define BACKGROUND_MASK_S L"\\background_mask.bmp"
#define BACKGROUND_MASK_L L"\\background_mask_l.bmp"

#define MIN_FONT_SIZE 5
#define MAX_FONT_SIZE 72

#define SAFE_DELETE(p) do \
{\
    if(p != nullptr) \
    { \
        delete p; \
        p = nullptr; \
    } \
} while (false)

//如果需要为Windows XP系统编译，请去掉下面一行代码的注释
//#define COMPILE_FOR_WINXP