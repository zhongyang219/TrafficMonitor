#pragma once
#include "afxcmn.h"
#include "Common.h"
#include "HistoryTrafficListDlg.h"
#include "HistoryTrafficCalendarDlg.h"

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

public:
	CHistoryTrafficListDlg m_tab1_dlg;
	CHistoryTrafficCalendarDlg m_tab2_dlg;

protected:
	deque<HistoryTraffic>& m_history_traffics;
	CSize m_min_size;
    CSize m_window_size{};

	CTabCtrl m_tab;
	int m_tab_selected;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void SetTabWndSize();

    void LoadConfig();
    void SaveConfig() const;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
