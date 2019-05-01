
// TrafficMonitor.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "Common.h"
#include "IniHelper.h"
#include "WinVersionHelper.h"
#include "SimpleXML.h"

// CTrafficMonitorApp: 
// 有关此类的实现，请参阅 TrafficMonitor.cpp
//


class CTrafficMonitorApp : public CWinApp
{
public:
	//各种路径
	wstring m_module_dir;		//程序exe文件的目录
	wstring m_appdata_dir;
	wstring m_module_path;		//程序exe文件的路径
	wstring m_module_path_reg;	//用于作为写入注册表开机自项的exe文件的路径（如果路径中有空格，加上引号）
	wstring m_config_path;
	wstring m_history_traffic_path;
	wstring m_log_path;
	wstring m_skin_path;
	wstring m_system_dir;
	wstring m_config_dir;

	//以下数据定义为App类中的公共成员，以便于在主对话框和任务栏窗口中都能访问
	unsigned int m_in_speed{};		//下载速度
	unsigned int m_out_speed{};		//上传速度
	int m_cpu_usage{};		//CPU利用率
	int m_memory_usage{};	//内存利用率
	int m_used_memory{};	//可用物理内存（单位为KB）
	int m_total_memory{};	//物理内存总量（单位为KB）

	__int64 m_today_up_traffic{};	//今天已使用的上传流量
	__int64 m_today_down_traffic{};	//今天已使用的下载流量

	bool m_cannot_save_config_warning{ true };	//指示是否会在无法保存设置时弹出提示框
	bool m_cannot_save_global_config_warning{ true };	//指示是否会在无法保存设置时弹出提示框

	//选项设置数据
	MainWndSettingData m_main_wnd_data;
	TaskBarSettingData m_taskbar_data;
	GeneralSettingData m_general_data;
	//其他设置数据
	MainConfigData m_cfg_data;
	int m_notify_interval;		//弹出通知消息的时间间隔
	bool m_debug_log;
	bool m_taksbar_transparent_color_enable;

	//bool m_is_windows10_fall_creator;
	CWinVersionHelper m_win_version;		//当前Windows的版本

	HICON m_notify_icons[MAX_NOTIFY_ICON];

public:
	CTrafficMonitorApp();

	void LoadConfig();
	void SaveConfig();

	void LoadGlobalConfig();
	void SaveGlobalConfig();

	int DPI(int pixel);
	void DPI(CRect& rect);
	void GetDPI(CWnd* pWnd);

	static void CheckUpdate(bool message);		//检查更新，如果message为true，则在检查时弹出提示信息
	//启动时检查更新线程函数
	static UINT CheckUpdateThreadFunc(LPVOID lpParam);

	void SetAutoRun(bool auto_run);
	bool GetAutoRun();

	//获取系统信息文本
	CString GetSystemInfoString();

private:
	//int m_no_multistart_warning_time{};		//用于设置在开机后多长时间内不弹出“已经有一个程序正在运行”的警告提示
	bool m_no_multistart_warning{};			//如果为false，则永远都不会弹出“已经有一个程序正在运行”的警告提示
	bool m_exit_when_start_by_restart_manager{ true };		//如果程序被Windows重启管理器重新启动，则退出程序
	int m_dpi{ 96 };
	CWinThread* m_pUpdateThread;			//检查更新的线程

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
	afx_msg void OnHelp();
};

extern CTrafficMonitorApp theApp;