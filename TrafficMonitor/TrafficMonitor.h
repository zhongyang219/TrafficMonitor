
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

	//选项设置数据
	//主窗口
	COLORREF m_text_color{};		//文字颜色
	CString m_font_name;	//字体名称
	int m_font_size;		//字体大小
	wstring m_up_string;		//默认为“上传: ”
	wstring m_down_string;		//默认为“下载: ”
	wstring m_cpu_string;		//默认为“CPU: ”
	wstring m_memory_string;		//默认为“内存: ”
	bool m_swap_up_down{ false };		//交换上传和下载显示的位置
	bool m_hide_main_wnd_when_fullscreen;		//有程序全屏运行时隐藏悬浮窗

	//任务栏窗口
	COLORREF m_tbar_back_color{ RGB(0,0,0) };	//背景颜色
	COLORREF m_tbar_text_color{ RGB(255,255,255) };	//文字颜色
	CString m_tbar_font_name;	//字体名称
	int m_tbar_font_size;		//字体大小
	wstring m_tbar_up_string;		//默认为“↑:”
	wstring m_tbar_down_string;		//默认为“↓:”
	wstring m_tbar_cpu_string;		//默认为“CPU:”
	wstring m_tbar_memory_string;		//默认为“内存:”
	bool m_tbar_swap_up_down{ false };		//交换上传和下载显示的位置

	bool m_tbar_show_cpu_memory;	//任务栏窗口显示CPU和内存利用率

	CTrafficMonitorApp();

	//bool WhenStart() const { return CCommon::WhenStart(m_no_multistart_warning_time); }
	void LoadConfig();
	void SaveConfig();

private:
	//int m_no_multistart_warning_time{};		//用于设置在开机后多长时间内不弹出“已经有一个程序正在运行”的警告提示
	bool m_no_multistart_warning{};			//如果为false，则永远都不会弹出“已经有一个程序正在运行”的警告提示

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CTrafficMonitorApp theApp;