
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
#include "OptionsDlg.h"
#include "PictureStatic.h"
#include "IconSelectDlg.h"
#include "DrawCommon.h"
#include "IniHelper.h"
#include "LinkStatic.h"
#include "AdapterCommon.h"
#include "AboutDlg.h"

// CTrafficMonitorDlg 对话框
class CTrafficMonitorDlg : public CDialogEx
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
	NOTIFYICONDATA m_ntIcon;	//通知区域图标
	CTaskBarDlg* m_tBarDlg;		//任务栏窗口的指针

	vector<NetWorkConection> m_connections;	//保存获取到的要显示到“选择网卡”菜单项中的所有网络连接
	MIB_IFTABLE* m_pIfTable;
	DWORD m_dwSize{};	//m_pIfTable的大小
	int m_connection_selected{ 0 };	//要显示流量的连接的序号
	unsigned __int64 m_in_bytes;		//当前已接收的字节数
	unsigned __int64 m_out_bytes;	//当前已发送的字节数
	unsigned __int64 m_last_in_bytes{};	//上次已接收的字节数
	unsigned __int64 m_last_out_bytes{};	//上次已发送的字节数

	FILETIME m_preidleTime;
	FILETIME m_prekernelTime;
	FILETIME m_preuserTime;

	bool m_first_start{ true };		//初始时为true，在定时器第一次启动后置为flase
	CRect m_screen_rect;		//屏幕的范围（不包含任务栏）
	LayoutData m_layout_data;
	CImage m_back_img;		//背景图片

	CStaticEx m_disp_up;	//显示上传的文本控件
	CStaticEx m_disp_down;	//显示下载的文本控件
	CStaticEx m_disp_cpu;	//显示CPU利用率的文本控件
	CStaticEx m_disp_memory;	//显示内存利用率的文本控件

	CFont m_font;			//字体

	int m_restart_cnt{ -1 };	//重新初始化次数
	unsigned int m_timer_cnt{};		//定时器触发次数（自程序启动以来的秒数）
	int m_zero_speed_cnt{};	//如果检测不到网速，该变量就会自加
	int m_insert_to_taskbar_cnt{};	//用来统计尝试嵌入任务栏的次数
	int m_cannot_intsert_to_task_bar_warning{ true };	//指示是否会在无法嵌入任务栏时弹出提示框

	static unsigned int m_WM_TASKBARCREATED;	//任务栏重启消息

	vector<wstring> m_skins;	//储存皮肤文件的路径
	int m_skin_selected{};		//选择的皮肤序号

	SYSTEMTIME m_start_time;	//程序启动时的时间
	deque<HistoryTraffic> m_history_traffics;	//储存历史流量

	CToolTipCtrl m_tool_tips;

	bool m_connection_change_flag{ false };		//如果执行过IniConnection()函数，该flag会置为true
	bool m_is_foreground_fullscreen{ false };	//指示前台窗口是否正在全局显示
	bool m_menu_popuped{ false };				//指示当前是否有菜单处于弹出状态


	void ShowInfo();		//将上传下载速度信息显示到窗口中
	CString GetMouseTipsInfo();		//获取鼠标提示信息
	void SetTransparency();			//根据m_transparency的值设置窗口透明度
	void SetTransparency(int transparency);
	void SetAlwaysOnTop();			//根据m_always_on_top的值设置窗口置顶
	void SetMousePenetrate();		//根据m_mouse_penetrate的值设置是否鼠标穿透
	void CheckWindowPos();			//测试窗口的位置，如窗口的位置在屏幕外，则移动窗口使其全部都在屏幕内，并返回新位置
	void GetScreenSize();			//获取屏幕的大小

	void AutoSelect();		//自动选择连接
	void IniConnection();	//初始化连接

	void IniConnectionMenu(CMenu* pMenu);	//初始化“选择网络连接”菜单
	void IniTaskBarConnectionMenu();		//初始化任务栏窗口的“选择网络连接”菜单
	void SetConnectionMenuState(CMenu* pMenu);		//设置“选择网络连接”菜单中选中的项目

	void CloseTaskBarWnd();	//关闭任务栏窗口
	void OpenTaskBarWnd();	//打开任务栏窗口

	void AddNotifyIcon();		//添加通知区图标
	void DeleteNotifyIcon();	
	void ShowNotifyTip(const wchar_t* title, const wchar_t* message);		//显示通知区提示
	void UpdateNotifyIconTip();		//更新通知区图标的鼠标提示

	void SaveHistoryTraffic();
	void LoadHistoryTraffic();

	void _OnOptions(int tab);	//打开“选项”对话框的处理，参数为打开时切换的标签

	void SetItemPosition();		//设置显示的4个项目的位置
	void GetSkinLayout();		//从当前皮肤获取布局数据

	void LoadBackGroundImage();
	void SetTextColor();
	void SetTextFont();

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
	afx_msg void OnShowMainWnd();
	afx_msg void OnUpdateShowMainWnd(CCmdUI *pCmdUI);
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
	afx_msg void OnCheckUpdate();
protected:
	afx_msg LRESULT OnTaskbarMenuPopedUp(WPARAM wParam, LPARAM lParam);
};
