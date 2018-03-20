#pragma once
#include "afxcmn.h"
#include "Common.h"
#include "ListCtrlEx.h"

// CHistoryTrafficDlg 对话框

class CHistoryTrafficDlg : public CDialog
{
	DECLARE_DYNAMIC(CHistoryTrafficDlg)

public:
	CHistoryTrafficDlg(deque<HistoryTraffic>& history_traffics, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHistoryTrafficDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HISTORY_TRAFFIC_DIALOG };
#endif

protected:
	CListCtrlEx m_history_list;
	deque<HistoryTraffic>& m_history_traffics;
	CSize m_min_size;

	CMenu m_menu;
	bool m_use_log_scale{ false };		//绘制表示历史流量数值的矩形时是否使用对数比例

	void SaveConfig() const;
	void LoadConfig();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnUseLinearScale();
	afx_msg void OnUseLogScale();
};
