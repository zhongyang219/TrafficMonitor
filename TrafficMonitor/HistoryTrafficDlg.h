#pragma once
#include "afxcmn.h"
#include "Common.h"
#include "ListCtrlEx.h"

// CHistoryTrafficDlg �Ի���

class CHistoryTrafficDlg : public CDialog
{
	DECLARE_DYNAMIC(CHistoryTrafficDlg)

public:
	CHistoryTrafficDlg(deque<HistoryTraffic>& history_traffics, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHistoryTrafficDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HISTORY_TRAFFIC_DIALOG };
#endif

protected:
	CListCtrlEx m_history_list;
	deque<HistoryTraffic>& m_history_traffics;
	CSize m_min_size;

	CMenu m_menu;
	bool m_use_log_scale{ false };		//���Ʊ�ʾ��ʷ������ֵ�ľ���ʱ�Ƿ�ʹ�ö�������

	void SaveConfig() const;
	void LoadConfig();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnUseLinearScale();
	afx_msg void OnUseLogScale();
};
