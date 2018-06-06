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

	//unsigned int m_in_bytes;		//当前查看的网络自启动以来已接收字节数
	//unsigned int m_out_bytes;		//当前查看的网络自启动以来已发送字节数

	SYSTEMTIME m_start_time;		//程序启动的时间

protected:

	//MIB_IFROW m_network_info;		//当前查看的网络连接信息
	//wstring m_ip_address{ L"-.-.-.-" };		//IP地址
	//wstring m_subnet_mask{ L"-.-.-.-" };	//子网掩码
	//wstring m_default_gateway{ L"-.-.-.-" };	//默认网关
	
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
};
