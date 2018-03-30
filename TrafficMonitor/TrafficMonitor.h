
// TrafficMonitor.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "Common.h"

// CTrafficMonitorApp: 
// 有关此类的实现，请参阅 TrafficMonitor.cpp
//


class CTrafficMonitorApp : public CWinApp
{
public:
	wstring m_module_path;
	wstring m_config_path;
	wstring m_history_traffic_path;
	wstring m_log_path;
	wstring m_skin_path;

	//以下数据定义为App类中的公共成员，以便于在主对话框和任务栏窗口中都能访问
	unsigned int m_in_speed{};		//下载速度
	unsigned int m_out_speed{};		//上传速度
	int m_cpu_usage{};		//CPU利用率
	int m_memory_usage{};	//内存利用率
	int m_used_memory{};	//可用物理内存（单位为KB）
	int m_total_memory{};	//物理内存总量（单位为KB）

	__int64 m_today_traffic{};	//今天已使用的流量

	bool m_hide_main_window;	//隐藏主窗口
	bool m_show_notify_icon{ true };	//显示通知区域图标
	bool m_tbar_show_cpu_memory;	//任务栏窗口显示CPU和内存利用率

	//选项设置数据
	MainWndSettingData m_main_wnd_data;
	TaskBarSettingData m_taskbar_data;
	GeneralSettingData m_general_data;

	bool m_is_windows10_fall_creator;

	HICON m_notify_icons[MAX_NOTIFY_ICON];

	CTrafficMonitorApp();

	//bool WhenStart() const { return CCommon::WhenStart(m_no_multistart_warning_time); }
	void LoadConfig();
	void SaveConfig();

	int DPI(int pixel);
	void DPI(CRect& rect);
	void GetDPI(CWnd* pWnd);

	static void CheckUpdate(bool message);		//检查更新，如果message为true，则在检查时弹出提示信息
	//启动时检查更新线程函数
	static UINT CheckUpdateThreadFunc(LPVOID lpParam);

	void SetAutoRun(bool auto_run);
	bool GetAutoRun();

private:
	//int m_no_multistart_warning_time{};		//用于设置在开机后多长时间内不弹出“已经有一个程序正在运行”的警告提示
	bool m_no_multistart_warning{};			//如果为false，则永远都不会弹出“已经有一个程序正在运行”的警告提示
	int m_dpi{ 96 };
	CWinThread* m_pUpdateThread;			//检查更新的线程

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CTrafficMonitorApp theApp;