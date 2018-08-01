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

//历史流量统计中用于指示不同范围内的流量的颜色
#define TRAFFIC_COLOR_BLUE RGB(0, 183, 238)
#define TRAFFIC_COLOR_GREEN RGB(128, 194, 105)
#define TRAFFIC_COLOR_YELLOE RGB(255, 216, 58)
#define TRAFFIC_COLOR_RED RGB(255, 95, 74)

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
	bool IsInvalid() const
	{
		return (up == NONE_STR && down == NONE_STR && cpu == NONE_STR && memory == NONE_STR);
	}
};

//鼠标双击窗口的动作
enum class DoubleClickAction
{
	CONNECTION_INFO,	//连接详情
	HISTORY_TRAFFIC,	//历史流量统计
	SHOW_MORE_INFO,		//显示更多信息
	OPTIONS,			//选项设置
	TASK_MANAGER,		//任务管理器
	CHANGE_SKIN,		//更换皮肤
	NONE				//不执行任何动作
};

//语言
enum class Language
{
	FOLLOWING_SYSTEM,		//跟随系统
	ENGLISH,				//英语
	SIMPLIFIED_CHINESE,		//简体中文
	TRADITIONAL_CHINESE		//繁体中文
};

//字体
struct FontInfo
{
	CString name;	//字体名称
	int size;		//字体大小
	bool bold;			//粗体
	bool italic;		//斜体
	bool underline;		//下划线
	bool strike_out;	//删除线
};

//将字号转成LOGFONT结构中的lfHeight
#define FONTSIZE_TO_LFHEIGHT(font_size) (-MulDiv(font_size, GetDeviceCaps(::GetDC(HWND_DESKTOP), LOGPIXELSY), 72))

//选项设置数据
struct MainConfigData
{
	bool m_always_on_top{ false };		//窗口置顶
	int m_transparency{ 100 };			//窗口透明度
	bool m_lock_window_pos{ false };	//锁定窗口位置
	bool m_show_more_info{ false };		//显示更多信息
	bool m_mouse_penetrate{ false };	//鼠标穿透
	bool m_show_task_bar_wnd{ false };	//显示任务栏窗口
	bool m_hide_main_window;			//隐藏主窗口
	bool m_show_notify_icon{ true };	//显示通知区域图标
	bool m_tbar_show_cpu_memory;		//任务栏窗口显示CPU和内存利用率

	int m_position_x;	//窗口位置的x坐标
	int m_position_y;	//窗口位置的y坐标

	bool m_auto_select{ false };	//自动选择连接
	bool m_select_all{ false };		//统计所有连接的网速
	string m_connection_name;		//当前选择网络的名称

	wstring m_skin_name;			//选择的皮肤的名称
	int m_notify_icon_selected{};	//要显示的通知区图标
	bool m_alow_out_of_border{ false };		//是否允许悬浮窗超出屏幕边界

	bool m_show_internet_ip{ false };		//是否在“连接详情”对话框中显示外网IP地址
	bool m_use_log_scale{ false };			//“历史流量统计”对话框中绘制表示历史流量数值的矩形时是否使用对数比例
};

//选项设置中“主窗口设置”和“任务栏窗口设置”中公共的数据（不使用此结构体创建对象）
struct PublicSettingData
{
	bool specify_each_item_color{ false };		//是否指定每个项目的颜色
	FontInfo font;			//字体
	DispStrings disp_str;	//显示的文本
	bool swap_up_down{ false };		//交换上传和下载显示的位置
	bool speed_short_mode{ false };		//网速显示简洁模式（减少小数点的位数，单位不显示“B”）
	bool separate_value_unit_with_space{ true };	//网速数值和单位用空格分隔
	bool unit_byte{ true };				//使用字节(B)而不是比特(b)为单位
	SpeedUnit speed_unit;		//网速的单位
	bool hide_unit;			//隐藏单位
	bool hide_percent;		//隐藏百分号
	DoubleClickAction double_click_action;		//鼠标双击动作
};

#define MAIN_WND_COLOR_NUM 4		//主窗口颜色数量
//选项设置中“主窗口设置”的数据
struct MainWndSettingData : public PublicSettingData
{
	COLORREF text_colors[MAIN_WND_COLOR_NUM]{};		//文字颜色（分别为“上传”、“下载”、“CPU”、“内存”的颜色）
	bool hide_main_wnd_when_fullscreen;		//有程序全屏运行时隐藏悬浮窗
};

#define TASKBAR_COLOR_NUM 8		//任务栏窗口颜色数量
//选项设置中“任务栏窗口设置”的数据
struct TaskBarSettingData : public PublicSettingData
{
	COLORREF  back_color{ RGB(0, 0, 0) };	//背景颜色
	COLORREF text_colors[TASKBAR_COLOR_NUM]{};		//文字颜色（依次为“上传”、“下载”、“CPU”、“内存”的标签和数据颜色）
	bool value_right_align{ false };	//数值是否右对齐
	int digits_number{ 4 };				//数据位数
	bool horizontal_arrange{ true };	//水平排列
	bool tbar_wnd_on_left{ false };		//如果为true，则任务栏窗口显示在任务栏的左侧（或上方）
};

//选项设置中“常规设置”的数据
struct GeneralSettingData
{
	bool check_update_when_start{ true };
	bool auto_run{ false };
	bool allow_skin_cover_font{ true };
	bool allow_skin_cover_text{ true };
	//通知消息
	bool traffic_tip_enable{ false };		//是否启用流量超出时提示
	int traffic_tip_value;					//要提示的流量临界值
	int traffic_tip_unit{};					//要提示的流量值的单位（0: MB, 1: GB）
	bool memory_usage_tip_enable{ false };	//是否启用内存使用率超出提示
	int memory_tip_value;			//要提示的内存使用率的临界值
	//语言
	Language language;

	bool show_all_interface{ true };
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


struct SkinData
{
	LayoutData layout;
	COLORREF text_colors[MAIN_WND_COLOR_NUM]{};		//文字颜色（分别为“上传”、“下载”、“CPU”、“内存”的颜色）
	bool specify_each_item_color;		//是否指定每个项目的颜色
	//wstring skin_author;
	DispStrings disp_str;
	//wstring font_name;
	//int font_size
	FontInfo font;			//字体
};
