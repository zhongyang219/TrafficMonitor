
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

	CTrafficMonitorApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CTrafficMonitorApp theApp;