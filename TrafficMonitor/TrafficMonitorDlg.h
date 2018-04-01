
// TrafficMonitorDlg.h : 头文件
//

#pragma once
#pragma comment (lib, "iphlpapi.lib")
#include"NetworkInfoDlg.h"
#include "afxwin.h"
#include "StaticEx.h"
#include "Common.h"
#include "TaskBarDlg.h"
#include "SkinDlg.h"
#include "HistoryTrafficDlg.h"
#include "DonateDlg.h"
#include "OptionsDlg.h"
#include "PictureStatic.h"
#include "IconSelectDlg.h"
#include "DrawCommon.h"

// CTrafficMonitorDlg 对话框
class CTrafficMonitorDlg : public CDialog
{
// 构造
public:
	CTrafficMonitorDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CTrafficMonitorDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRAFFICMONITOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CMenu m_menu;		//右键菜单对象
	CMenu* m_select_connection_menu;	//“选择网卡”菜单项
	NOTIFYICONDATA m_ntIcon;	//通知区域图标
	CTaskBarDlg* m_tBarDlg;		//任务栏窗口的指针

	//嵌套结构体，保存一个网络连接的索引和描述等信息
	struct NetWorkConection
	{
		int index;				//网络索引
		string description;		//网络描述
		unsigned int in_bytes;	//初始时已接收字节数
		unsigned int out_bytes;	//初始时已发送字节数
		NetWorkConection(int idx, string desc, unsigned int in_bytes, unsigned out_bytes)
			: index{ idx }, description{ desc }, in_bytes{ in_bytes }, out_bytes{out_bytes}		//构造函数
		{}
	};

	vector<NetWorkConection> m_connections;	//保存获取到的要显示到“选择网卡”菜单项中的所有网络连接
	MIB_IFTABLE* m_pIfTable;
	DWORD m_dwSize{};	//m_pIfTable的大小
	int m_connection_selected{ 0 };	//要显示流量的连接的序号
	bool m_auto_select{ false };	//自动选择连接
	unsigned int m_in_bytes;		//当前已接收的字节数
	unsigned int m_out_bytes;	//当前已发送的字节数
	unsigned int m_last_in_bytes{};	//上次已接收的字节数
	unsigned int m_last_out_bytes{};	//上次已发送的字节数
	string m_connection_name;	//当前选择网络的名称

	FILETIME m_preidleTime;
	FILETIME m_prekernelTime;
	FILETIME m_preuserTime;

	bool m_always_on_top{ false };		//窗口置顶
	int m_transparency{ 100 };			//窗口透明度
	bool m_lock_window_pos{ false };	//锁定窗口位置
	bool m_show_more_info{ false };		//显示更多信息
	bool m_mouse_penetrate{ false };	//鼠标穿透
	bool m_show_task_bar_wnd{ false };	//显示任务栏窗口
	//bool m_swap_up_down{ false };		//交换上传和下载显示的位置
	//bool m_auto_run;				//是否开机自动运行
	//wstring m_start_up_path;		//开始菜单中的“启动”文件夹的路径

	bool m_first_start{ true };		//初始时为true，在定时器第一次启动后置为flase

	int m_position_x;	//窗口位置的x坐标
	int m_position_y;	//窗口位置的y坐标

	//int m_screen_width;		//屏幕宽度
	//int m_screen_height;	//屏幕高度
	CRect m_screen_rect;		//屏幕的范围（不包含任务栏）

	//int m_window_height;	//窗口高度
	//int m_window_height_s;	//不显示CPU和内存利用率时的窗口高度
	LayoutData m_layout_data;

	CImage m_back_img;		//背景图片

	//CStaticEx m_disp_up;	//显示上传的文本控件
	//CStaticEx m_disp_down;	//显示下载的文本控件
	//CStaticEx m_disp_cpu;	//显示CPU利用率的文本控件
	//CStaticEx m_disp_memory;	//显示内存利用率的文本控件
	CRect m_rect_up;
	CRect m_rect_down;
	CRect m_rect_cpu;
	CRect m_rect_memory;

	//COLORREF m_text_color{};		//文字颜色
	CFont m_font;			//字体
	//CString m_font_name;	//字体名称
	//int m_font_size;		//字体大小

	int m_restart_cnt{ -1 };	//重新初始化次数
	unsigned int m_timer_cnt{};		//定时器触发次数
	int m_zero_speed_cnt{};	//如果检测不到网速，该变量就会自加
	int m_insert_to_taskbar_cnt{};	//用来统计尝试嵌入任务栏的次数
	int m_cannot_intsert_to_task_bar_warning{ true };	//指示是否会在无法嵌入任务栏时弹出提示框

	static unsigned int m_WM_TASKBARCREATED;	//任务栏重启消息

	vector<wstring> m_skins;	//储存皮肤文件的路径
	int m_skin_selected{};		//选择的皮肤序号
	wstring m_skin_name;		//选择的皮肤的名称

	int m_notify_icon_selected{};	//要显示的通知区图标
	bool m_alow_out_of_border{ false };		//是否允许悬浮窗超出屏幕边界

	SYSTEMTIME m_start_time;	//程序启动时的时间
	deque<HistoryTraffic> m_history_traffics;	//储存历史流量

	CToolTipCtrl m_tool_tips;

	bool m_connection_change_flag{ false };		//如果执行过IniConnection()函数，该flag会置为true
	bool m_cannot_save_config_warning{ true };	//指示是否会在无法保存设置时弹出提示框
	bool m_is_foreground_fullscreen{ false };	//指示前台窗口是否正在全局显示
	bool m_menu_popuped{ false };				//指示当前是否有菜单处于弹出状态


	void DrawInfo(CDrawCommon* draw);		//将上传下载速度信息显示到窗口中
	CString GetMouseTipsInfo();		//获取鼠标提示信息
	void SetTransparency();			//根据m_transparency的值设置窗口透明度
	void SetTransparency(int transparency);
	void SetAlwaysOnTop();			//根据m_always_on_top的值设置窗口置顶
	void SetMousePenetrate();		//根据m_mouse_penetrate的值设置是否鼠标穿透
	void CheckWindowPos();			//测试窗口的位置，如窗口的位置在屏幕外，则移动窗口使其全部都在屏幕内，并返回新位置
	void GetScreenSize();			//获取屏幕的大小

	void LoadConfig();	//载入设置
	void SaveConfig();	//保存设置

	void AutoSelect();		//自动选择连接
	void IniConnection();	//初始化连接

	void CloseTaskBarWnd();	//关闭任务栏窗口
	void OpenTaskBarWnd();	//打开任务栏窗口

	void AddNotifyIcon();		//添加通知区图标
	void DeleteNotifyIcon();	

	void SaveHistoryTraffic();
	void LoadHistoryTraffic();

	void _OnOptions(int tab);	//打开“选项”对话框的处理，参数为打开时切换的标签

	void SetItemPosition();		//设置显示的4个项目的位置
	void GetSkinLayout();		//从当前皮肤获取布局数据

	void LoadBackGroundImage();

public:
	void ApplySettings();

protected:
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
//	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNetworkInfo();
	afx_msg void OnAlwaysOnTop();
	afx_msg void OnUpdateAlwaysOnTop(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnTransparency100();
	afx_msg void OnTransparency80();
	afx_msg void OnTransparency60();
	afx_msg void OnTransparency40();
	afx_msg void OnClose();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnInitMenu(CMenu* pMenu);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLockWindowPos();
	afx_msg void OnUpdateLockWindowPos(CCmdUI *pCmdUI);
	afx_msg void OnMove(int x, int y);
protected:
	afx_msg LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnShowNotifyIcon();
	afx_msg void OnUpdateShowNotifyIcon(CCmdUI *pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnShowCpuMemory();
	afx_msg void OnUpdateShowCpuMemory(CCmdUI *pCmdUI);
	afx_msg void OnMousePenetrate();
	afx_msg void OnUpdateMousePenetrate(CCmdUI *pCmdUI);
	//afx_msg void OnTextColor();
	afx_msg void OnShowTaskBarWnd();
	afx_msg void OnUpdateShowTaskBarWnd(CCmdUI *pCmdUI);
	afx_msg void OnAppAbout();
	afx_msg void OnShowCpuMemory2();
//	afx_msg void OnAutoRunWhenStart();
	afx_msg void OnHideMainWnd();
	afx_msg void OnUpdateHideMainWnd(CCmdUI *pCmdUI);
	afx_msg void OnChangeSkin();
//	afx_msg void OnUpdateAutoRunWhenStart(CCmdUI *pCmdUI);
	afx_msg LRESULT OnTaskBarCreated(WPARAM wParam, LPARAM lParam);
	//afx_msg void OnSetFont();
	afx_msg void OnTrafficHistory();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnOptions();
	afx_msg void OnOptions2();
protected:
	afx_msg LRESULT OnExitmenuloop(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnChangeNotifyIcon();
	afx_msg void OnAlowOutOfBorder();
	afx_msg void OnUpdateAlowOutOfBorder(CCmdUI *pCmdUI);
	afx_msg void OnPaint();
};
