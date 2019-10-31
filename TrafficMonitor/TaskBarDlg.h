#pragma once
#include "Common.h"
#include "afxwin.h"
//#include "StaticEx.h"
#include "DrawCommon.h"
#include "IniHelper.h"
#include "CommonData.h"

// CTaskBarDlg 对话框
#define TASKBAR_WND_HEIGHT theApp.DPI(32)				//任务栏窗口的高度
#define WM_TASKBAR_MENU_POPED_UP (WM_USER + 1004)		//定义任务栏窗口右键菜单弹出时发出的消息

class CTaskBarDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskBarDlg)

public:
	CTaskBarDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTaskBarDlg();

	CToolTipCtrl m_tool_tips;
	CMenu m_menu;	//右键菜单

	void ShowInfo(CDC* pDC); 	//将信息绘制到控件上
	void TryDrawStatusBar(CDrawCommon& drawer, const CRect& rect_bar, int usage_percent); //绘制CPU/内存状态条

	bool AdjustWindowPos();	//设置窗口在任务栏中的位置

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASK_BAR_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	HWND m_hTaskbar;	//任务栏窗口句柄
	HWND m_hBar;		//任务栏窗口二级容器的句柄
	HWND m_hMin;		//最小化窗口的句柄
	CRect m_rcBar;		//初始状态时任务栏窗口的矩形区域
	CRect m_rcMin;		//初始状态时最小化窗口的矩形区域
	CRect m_rect;		//当前窗口的矩形区域
	int m_window_width;		//窗口宽度
	int m_window_width_s;	//不显示CPU和内存利用率时的窗口宽度
	int m_window_height;
	int m_up_lable_width;	//上传标签的宽度
	int m_down_lable_width;	//下载标签的宽度
	int m_cpu_lable_width;		//CPU标签的宽度
	int m_memory_lable_width;	//内存标签的宽度
	int m_ud_value_width;		//上传、下载数值的宽度
	int m_cm_value_width;		//CPU、内存数值的宽度

	int m_min_bar_width;	//最小化窗口缩小宽度后的宽度
	int m_min_bar_height;	//最小化窗口缩小高度后的高度（用于任务栏在屏幕左侧或右侧时）

	int m_left_space{};			//最小化窗口和二级窗口窗口左侧的边距
	int m_top_space{};			//最小化窗口和二级窗口窗口顶部的边距（用于任务栏在屏幕左侧或右侧时）

	bool m_connot_insert_to_task_bar{ false };	//如果窗口无法嵌入任务栏，则为true
	bool m_taskbar_on_top_or_bottom{ true };		//如果任务栏在屏幕顶部或底部，则为ture
	int m_error_code{};

	CFont m_font;			//字体

	CDC* m_pDC{};		//窗口的DC，用来计算窗口的宽度

	bool IsTaskbarOnTopOrBottom();		//判断任务栏是否在屏幕的顶部或底部，如果是则返回false，如果任务栏在屏幕两侧，则返回false
	CString GetMouseTipsInfo();		//获取鼠标提示

public:
	void SetTextFont();
	void ApplySettings();
	void CalculateWindowWidth();		//计算窗口需要的宽度

	void SetToolTipsTopMost();			//设置鼠标提示置顶
	void UpdateToolTips();

	bool GetCannotInsertToTaskBar() const { return m_connot_insert_to_task_bar; }
	int GetErrorCode() const { return m_error_code; }

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//afx_msg void OnSetBackgroundColor();
	//afx_msg void OnSetTextColor();
	afx_msg void OnInitMenu(CMenu* pMenu);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//afx_msg void OnSetFont();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
};
