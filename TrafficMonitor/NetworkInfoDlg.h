#pragma once
#include"Common.h"
#include "afxcmn.h"

// CNetworkInfoDlg 对话框

class CNetworkInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CNetworkInfoDlg)

public:
	CNetworkInfoDlg(MIB_IFROW network_info ,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNetworkInfoDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NETWORK_INFO_DIALOG };
#endif

	//void SetInOutBytes(unsigned int in_bytes, unsigned int out_bytes)
	//{
	//	m_in_bytes = in_bytes;
	//	m_out_bytes = out_bytes;
	//}
	unsigned int m_in_bytes;		//当前查看的网络自启动以来已接收字节数
	unsigned int m_out_bytes;		//当前查看的网络自启动以来已发送字节数

	SYSTEMTIME m_start_time;		//程序启动的时间

protected:

	MIB_IFROW m_network_info;		//当前查看的网络连接信息
	wstring m_ip_address{ L"-.-.-.-" };		//IP地址
	wstring m_subnet_mask{ L"-.-.-.-" };	//子网掩码
	wstring m_default_gateway{ L"-.-.-.-" };	//默认网关
	
	CListCtrl m_info_list;

	void GetIPAddress();	//获取IP地址
	void ShowInfo();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
};
