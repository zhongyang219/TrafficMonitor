#pragma once
#include"Common.h"
#include "afxcmn.h"
#include "AdapterCommon.h"

// CNetworkInfoDlg 对话框

class CNetworkInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CNetworkInfoDlg)

public:
	CNetworkInfoDlg(vector<NetWorkConection>& adapters, MIB_IFROW* pIfRow, int connection_selected, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNetworkInfoDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NETWORK_INFO_DIALOG };
#endif

	SYSTEMTIME m_start_time;		//程序启动的时间

protected:

	vector<NetWorkConection>& m_connections;
	MIB_IFROW* m_pIfRow;
	int m_connection_selected;

	CListCtrl m_info_list;
	CMenu m_menu;
	CString m_selected_string;
	CSize m_min_size;

	CWinThread* m_pGetIPThread;			//获取外网IP的线程

	//void GetIPAddress();	//获取IP地址
	void ShowInfo();
	void GetProgramElapsedTime();

	//获取外网IP的线程函数
	static UINT GetInternetIPThreadFunc(LPVOID lpParam);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
public:
	afx_msg void OnCopyText();
	afx_msg void OnNMRClickInfoList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	afx_msg void OnBnClickedPreviousButton();
	afx_msg void OnBnClickedNextButton();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
