
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

//选项设置数据
struct MainWndSettingData
{
	//主窗口
	COLORREF text_color{};		//文字颜色
	CString font_name;	//字体名称
	int font_size;		//字体大小
	wstring up_string;		//默认为“上传: ”
	wstring down_string;		//默认为“下载: ”
	wstring cpu_string;		//默认为“CPU: ”
	wstring memory_string;		//默认为“内存: ”
	bool swap_up_down{ false };		//交换上传和下载显示的位置
	bool hide_main_wnd_when_fullscreen;		//有程序全屏运行时隐藏悬浮窗
};

struct TaskBarSettingData
{
	//任务栏窗口
	COLORREF  back_color{ RGB(0,0,0) };	//背景颜色
	COLORREF  text_color{ RGB(255,255,255) };	//文字颜色
	CString  font_name;	//字体名称
	int  font_size;		//字体大小
	wstring  up_string;		//默认为“↑:”
	wstring  down_string;		//默认为“↓:”
	wstring  cpu_string;		//默认为“CPU:”
	wstring  memory_string;		//默认为“内存:”
	bool  swap_up_down{ false };		//交换上传和下载显示的位置
	bool tbar_wnd_on_left{ false };		//如果为true，则任务栏窗口显示在任务栏的左侧（或上方）
};

struct GeneralSettingData
{
	//常规设置
	bool check_update_when_start{ true };
	bool auto_run{ false };
};

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

	CTrafficMonitorApp();

	//bool WhenStart() const { return CCommon::WhenStart(m_no_multistart_warning_time); }
	void LoadConfig();
	void SaveConfig();

	int DPI(int pixel);
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