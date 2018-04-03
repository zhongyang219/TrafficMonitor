//此文件包含全局结构体、枚举类型的定义
#pragma once
#include "stdafx.h"

//储存某一天的历史流量
struct HistoryTraffic
{
	int year;
	int month;
	int day;
	unsigned int kBytes;	//当天使用的流量（以KB为单位）

	//比较两个HistoryTraffic对象的日期，如果a的时间大于b，则返回true
	static bool DateGreater(const HistoryTraffic& a, const HistoryTraffic& b)
	{
		if (a.year != b.year)
			return a.year > b.year;
		else if (a.month != b.month)
			return a.month > b.month;
		else if (a.day != b.day)
			return a.day > b.day;
		else
			return false;
	}

	//判断两个HistoryTraffic对象的日期是否相等
	static bool DateEqual(const HistoryTraffic& a, const HistoryTraffic& b)
	{
		return a.year == b.year && a.month == b.month && a.day == b.day;
	}
};

//网速单位
enum class SpeedUnit
{
	AUTO,		//自动
	KBPS,		//KB/s
	MBPS		//MB/s
};

#define DEF_CH L'\"'		//写入和读取ini文件字符串时，在字符串前后添加的字符
#define NONE_STR L"@@@"		//用于指定一个无效字符串
struct DispStrings		//显示的文本
{
	wstring up;
	wstring down;
	wstring cpu;
	wstring memory;
	void operator=(const DispStrings& disp_str)		//重载赋值运算符
	{
		//如果赋值的字符串是定义的无效字符串，则不赋值
		if (disp_str.up != NONE_STR)
			up = disp_str.up;
		if (disp_str.down != NONE_STR)
			down = disp_str.down;
		if (disp_str.cpu != NONE_STR)
			cpu = disp_str.cpu;
		if (disp_str.memory != NONE_STR)
			memory = disp_str.memory;
	}
};

//选项设置数据
struct MainWndSettingData
{
	//主窗口
	COLORREF text_color{};		//文字颜色
	CString font_name;	//字体名称
	int font_size;		//字体大小
	DispStrings disp_str;	//显示的文本
	bool speed_short_mode{ false };		//网速显示简洁模式（减少小数点的位数，单位不显示“B”）
	bool swap_up_down{ false };		//交换上传和下载显示的位置
	bool hide_main_wnd_when_fullscreen;		//有程序全屏运行时隐藏悬浮窗
	SpeedUnit m_speed_unit;		//网速的单位
	bool m_hide_unit;			//隐藏单位
};

struct TaskBarSettingData
{
	//任务栏窗口
	COLORREF  back_color{ RGB(0, 0, 0) };	//背景颜色
	COLORREF  text_color{ RGB(255, 255, 255) };	//文字颜色
	CString  font_name;	//字体名称
	int  font_size;		//字体大小
	DispStrings disp_str;	//显示的文本
	bool swap_up_down{ false };		//交换上传和下载显示的位置
	bool speed_short_mode{ false };		//网速显示简洁模式（减少小数点的位数，单位不显示“B”）
	bool tbar_wnd_on_left{ false };		//如果为true，则任务栏窗口显示在任务栏的左侧（或上方）
	SpeedUnit m_speed_unit;		//网速的单位
	bool m_hide_unit;			//隐藏单位
};

struct GeneralSettingData
{
	//常规设置
	bool check_update_when_start{ true };
	bool auto_run{ false };
};

enum class Alignment
{
	LEFT,
	RIGHT,
	CENTER
};

//主窗口布局数据
struct LayoutData
{
	int text_height;	//文本区域的高度
	bool no_text;		//不显示数值前的“上传”、“下载”等文本
	int preview_width;		//预览区画布宽度
	int preview_height;		//预览区画布高度

	//大窗口模式（勾选“显示更多信息”时的布局）
	int width_l;	//窗口的宽度
	int height_l;	//窗口的高度
	int up_x_l;		//“上传”的x坐标
	int up_y_l;		//“上传”的y坐标
	int up_width_l;		//“上传”的宽度
	Alignment up_align_l;	//“上传”的对齐方式
	bool show_up_l;			//是否显示“上传”
	int down_x_l;			//“下载”的x坐标
	int down_y_l;			//“下载”的y坐标
	int down_width_l;		//“下载”的宽度
	Alignment down_align_l;	//“下载”的对齐方式
	bool show_down_l;		//是否显示“下载”
	int cpu_x_l;			//“CPU”的x坐标
	int cpu_y_l;			//“CPU”的y坐标
	int cpu_width_l;		//“CPU”的宽度
	Alignment cpu_align_l;	//“CPU”的对齐方式
	bool show_cpu_l;		//是否显示“CPU”
	int memory_x_l;			//“内存”的x坐标
	int memory_y_l;			//“内存”的y坐标
	int memory_width_l;		//“内存”的宽度
	Alignment memory_align_l;	//“内存”的对齐方式
	bool show_memory_l;		//是否显示“内存”
	int preview_x_l;		//在“选择皮肤”界面中预览图的x坐标
	int preview_y_l;		//在“选择皮肤”界面中预览图的y坐标

	//小窗口模式（不勾选“显示更多信息”时的布局）
	int width_s;	//窗口的宽度
	int height_s;	//窗口的高度
	int up_x_s;		//“上传”的x坐标
	int up_y_s;		//“上传”的y坐标
	int up_width_s;		//“上传”的宽度
	Alignment up_align_s;	//“上传”的对齐方式
	bool show_up_s;			//是否显示“上传”
	int down_x_s;			//“下载”的x坐标
	int down_y_s;			//“下载”的y坐标
	int down_width_s;		//“下载”的宽度
	Alignment down_align_s;	//“下载”的对齐方式
	bool show_down_s;		//是否显示“下载”
	int cpu_x_s;			//“CPU”的x坐标
	int cpu_y_s;			//“CPU”的y坐标
	int cpu_width_s;		//“CPU”的宽度
	Alignment cpu_align_s;	//“CPU”的对齐方式
	bool show_cpu_s;		//是否显示“CPU”
	int memory_x_s;			//“内存”的x坐标
	int memory_y_s;			//“内存”的y坐标
	int memory_width_s;		//“内存”的宽度
	Alignment memory_align_s;	//“内存”的对齐方式
	bool show_memory_s;		//是否显示“内存”
	int preview_x_s;	//在“选择皮肤”界面中预览图的x坐标
	int preview_y_s;	//在“选择皮肤”界面中预览图的y坐标
};
