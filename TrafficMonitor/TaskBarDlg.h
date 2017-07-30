#pragma once
#include "Common.h"
#include "afxwin.h"
#include "StaticEx.h"

// CTaskBarDlg 对话框

class CTaskBarDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskBarDlg)

public:
	CTaskBarDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTaskBarDlg();

	//unsigned int m_in_speed{};		//下载速度
	//unsigned int m_out_speed{};		//上传速度
	//int m_cpu_usage{};		//CPU利用率
	//int m_memory_usage{};	//内存利用率
	//int m_used_memory{};	//可用物理内存（单位为KB）
	//int m_total_memory{};	//物理内存总量（单位为KB）

	//__int64 m_today_traffic{};	//今天已使用的流量

	CToolTipCtrl m_tool_tips;
	//int m_tip_type{};	//用于指示要显示的提示的类型，0：不显示提示；1：显示今日已用流量；2：内存利用详情

	bool m_show_cpu_memory;	//显示CPU和内存利用率
	//bool m_hide_main_window;	//隐藏主窗口
	//bool m_position_corrected{ false };	//如果窗口位置被修正过，则为true

	void ShowInfo();		//将信息显示到控件上
	bool AdjustWindowPos();	//设置窗口在任务栏中的位置
	//void ShowCpuMemory();	//切换显示CPU和内存利用率时的处理

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASK_BAR_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	HWND m_hTaskbar;	//任务栏窗口句柄
	HWND m_hBar;		//任务栏窗口二级容器的句柄
	HWND m_hMin;		//最小化窗口的句柄
	CRect m_rcBar;		//任务栏窗口的矩形区域
	CRect m_rcMin;		//最小化窗口的矩形区域
	CRect m_rect;		//当前窗口的矩形区域
	int m_window_width;		//窗口宽度
	int m_window_width_s;	//不显示CPU和内存利用率时的窗口宽度

	int m_min_bar_width;	//最小化窗口缩小宽度后的宽度
	int m_min_bar_height;	//最小化窗口缩小高度后的高度（用于任务栏在屏幕左侧或右侧时）

	bool m_connot_insert_to_task_bar{ false };	//如果窗口无法嵌入任务栏，则为true

	CStaticEx m_disp_up;	//显示上传速度的Static控件
	CStaticEx m_disp_down;	//显示下载速度的Static控件
	CStaticEx m_disp_cpu;	//显示CPU利用率的Static控件
	CStaticEx m_disp_memory;	//显示内存利用率的Static控件

	CMenu m_menu;	//右键菜单

	COLORREF m_back_color{ RGB(0,0,0) };	//背景颜色
	COLORREF m_text_color{ RGB(255,255,255) };	//文字颜色
	CFont m_font;			//字体
	CString m_font_name;	//字体名称
	int m_font_size;		//字体大小

	bool m_swap_up_down{ false };		//交换上传和下载显示的位置

	void SaveConfig();		//保存设置到ini文件
	void LoadConfig();		//从ini文件载入设置

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetBackgroundColor();
	afx_msg void OnSetTextColor();
	afx_msg void OnInitMenu(CMenu* pMenu);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSetFont();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
