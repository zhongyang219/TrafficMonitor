// CNetworkInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "NetworkInfoDlg.h"
#include "afxdialogex.h"


// CNetworkInfoDlg 对话框

IMPLEMENT_DYNAMIC(CNetworkInfoDlg, CDialog)

CNetworkInfoDlg::CNetworkInfoDlg(MIB_IFROW network_info, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_NETWORK_INFO_DIALOG, pParent), m_network_info(network_info)
{

}

CNetworkInfoDlg::~CNetworkInfoDlg()
{
}


void CNetworkInfoDlg::GetIPAddress()
{
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();		//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);		//得到结构体大小,用于GetAdaptersInfo参数
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量

	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		delete pIpAdapterInfo;	//释放原来的内存空间
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];	//重新申请内存空间用来存储所有网卡信息
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
	}

	PIP_ADAPTER_INFO pIpAdapterInfoHead = pIpAdapterInfo;	//保存pIpAdapterInfo链表中第一个元素的地址
	if (ERROR_SUCCESS == nRel)
	{
		string current_network_descr{ (const char*)m_network_info.bDescr };		//MIB_IFROW结构中的当前选中的网络连接的描述
		//获取网卡信息
		//可能有多网卡,因此通过循环来查找当前要显示的网卡
		while (pIpAdapterInfo)
		{
			if(current_network_descr.find(pIpAdapterInfo->Description) != string::npos)
			{
				m_ip_address = CCommon::StrToUnicode(pIpAdapterInfo->IpAddressList.IpAddress.String);
				m_subnet_mask = CCommon::StrToUnicode(pIpAdapterInfo->IpAddressList.IpMask.String);
				m_default_gateway = CCommon::StrToUnicode(pIpAdapterInfo->GatewayList.IpAddress.String);
				break;
			}
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
	}
	//释放内存空间
	if (pIpAdapterInfoHead)
	{
		delete pIpAdapterInfoHead;
	}
}

void CNetworkInfoDlg::ShowInfo()
{
	CString temp;
	m_info_list.InsertItem(0, CCommon::LoadText(IDS_INTERFACE_NAME));
	m_info_list.SetItemText(0, 1, m_network_info.wszName);

	m_info_list.InsertItem(1, CCommon::LoadText(IDS_INTERFACE_DESCRIPTION));
	m_info_list.SetItemText(1, 1, CCommon::StrToUnicode((const char*)m_network_info.bDescr).c_str());

	m_info_list.InsertItem(2, CCommon::LoadText(IDS_CONNECTION_TYPE));
	switch (m_network_info.dwType)
	{
	case IF_TYPE_OTHER: temp = CCommon::LoadText(IDS_IF_TYPE_OTHER); break;
	case IF_TYPE_ETHERNET_CSMACD: temp = CCommon::LoadText(IDS_IF_TYPE_ETHERNET_CSMACD); break;
	case IF_TYPE_ISO88025_TOKENRING: temp = CCommon::LoadText(IDS_IF_TYPE_ISO88025_TOKENRING); break;
	case IF_TYPE_FDDI: temp = CCommon::LoadText(IDS_IF_TYPE_FDDI); break;
	case IF_TYPE_PPP: temp = CCommon::LoadText(IDS_IF_TYPE_PPP); break;
	case IF_TYPE_SOFTWARE_LOOPBACK: temp = CCommon::LoadText(IDS_IF_TYPE_SOFTWARE_LOOPBACK); break;
	case IF_TYPE_ATM: temp = CCommon::LoadText(IDS_IF_TYPE_ATM); break;
	case IF_TYPE_IEEE80211: temp = CCommon::LoadText(IDS_IF_TYPE_IEEE80211); break;
	case IF_TYPE_TUNNEL: temp = CCommon::LoadText(IDS_IF_TYPE_TUNNEL); break;
	case IF_TYPE_IEEE1394: temp = CCommon::LoadText(IDS_IF_TYPE_IEEE1394); break;
	case IF_TYPE_IEEE80216_WMAN: temp = CCommon::LoadText(IDS_IF_TYPE_IEEE80216_WMAN); break;
	case IF_TYPE_WWANPP: temp = CCommon::LoadText(IDS_IF_TYPE_WWANPP); break;
	case IF_TYPE_WWANPP2: temp = CCommon::LoadText(IDS_IF_TYPE_WWANPP2); break;
	default: temp = CCommon::LoadText(IDS_UNKNOW_CONNECTION); break;
	}
	m_info_list.SetItemText(2, 1, temp);

	//m_info_list.InsertItem(3, _T("最大传输单位大小"));
	//temp.Format(_T("%u"), m_network_info.dwMtu);
	//m_info_list.SetItemText(3, 1, temp);

	m_info_list.InsertItem(3, CCommon::LoadText(IDS_SPEED));
	temp.Format(_T("%dMbps"), m_network_info.dwSpeed / 1000000);
	m_info_list.SetItemText(3, 1, temp);

	m_info_list.InsertItem(4, CCommon::LoadText(IDS_ADAPTER_PHYSICAL_ADDRESS));
	temp = _T("");
	char buff[3];
	for (size_t i{}; i < m_network_info.dwPhysAddrLen; i++)
	{
		//_itoa_s(m_network_info.bPhysAddr[i], buff, 16);
		sprintf_s(buff, "%.2x", m_network_info.bPhysAddr[i]);
		temp += buff;
		if (i != m_network_info.dwPhysAddrLen - 1)
			temp += _T('-');
	}
	m_info_list.SetItemText(4, 1, temp);

	m_info_list.InsertItem(5, CCommon::LoadText(IDS_IP_ADDRESS));
	m_info_list.SetItemText(5, 1, m_ip_address.c_str());

	m_info_list.InsertItem(6, CCommon::LoadText(IDS_SUBNET_MASK));
	m_info_list.SetItemText(6, 1, m_subnet_mask.c_str());

	m_info_list.InsertItem(7, CCommon::LoadText(IDS_DEFAULT_GATEWAY));
	m_info_list.SetItemText(7, 1, m_default_gateway.c_str());

	////temp.Format(_T("物理地址长度：%d\r\n"), m_network_info.dwPhysAddrLen);
	////out_info += temp;
	////temp = _T("适配器物理地址：");
	////temp += StrToUnicode((const char*)m_network_info.bPhysAddr).c_str();
	////out_info += temp;
	////out_info += _T("\r\n");

	//m_info_list.InsertItem(5, _T("管理员状态"));
	//m_info_list.SetItemText(5, 1, m_network_info.dwAdminStatus ? _T("启用") : _T("禁用"));

	m_info_list.InsertItem(8, CCommon::LoadText(IDS_OPERATIONAL_STATUS));
	switch (m_network_info.dwOperStatus)
	{
	case IF_OPER_STATUS_NON_OPERATIONAL: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_NON_OPERATIONAL); break;
	case IF_OPER_STATUS_UNREACHABLE: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_UNREACHABLE); break;
	case IF_OPER_STATUS_DISCONNECTED: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_DISCONNECTED); break;
	case IF_OPER_STATUS_CONNECTING: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_CONNECTING); break;
	case IF_OPER_STATUS_CONNECTED: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_CONNECTED); break;
	case IF_OPER_STATUS_OPERATIONAL: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_OPERATIONAL); break;
	default: temp = CCommon::LoadText(IDS_UNKNOW_STATUS); break;
	}
	m_info_list.SetItemText(8, 1, temp);

	m_info_list.InsertItem(9, CCommon::LoadText(IDS_BYTES_RECEIVED));
	temp.Format(_T("%u (%s)"), m_network_info.dwInOctets, CCommon::DataSizeToString(m_network_info.dwInOctets));
	m_info_list.SetItemText(9, 1, temp);

	m_info_list.InsertItem(10, CCommon::LoadText(IDS_BYTES_SENT));
	temp.Format(_T("%u (%s)"), m_network_info.dwOutOctets, CCommon::DataSizeToString(m_network_info.dwOutOctets));
	m_info_list.SetItemText(10, 1, temp);

	m_info_list.InsertItem(11, CCommon::LoadText(IDS_BYTES_RECEIVED_SINCE_START));
	temp.Format(_T("%u (%s)"), m_in_bytes, CCommon::DataSizeToString(m_in_bytes));
	m_info_list.SetItemText(11, 1, temp);

	m_info_list.InsertItem(12, CCommon::LoadText(IDS_BYTES_SENT_SINCE_START));
	temp.Format(_T("%u (%s)"), m_out_bytes, CCommon::DataSizeToString(m_out_bytes));
	m_info_list.SetItemText(12, 1, temp);

	m_info_list.InsertItem(13, CCommon::LoadText(IDS_PROGRAM_ELAPSED_TIME));
	SYSTEMTIME current_time, time;
	GetLocalTime(&current_time);
	time = CCommon::CompareSystemTime(current_time, m_start_time);
	temp.Format(CCommon::LoadText(IDS_HOUR_MINUTE_SECOND), time.wHour, time.wMinute, time.wSecond);
	m_info_list.SetItemText(13, 1, temp);

	m_info_list.InsertItem(14, CCommon::LoadText(IDS_INTERNET_IP_ADDRESS));
	m_info_list.SetItemText(14, 1, m_internet_ip_address.c_str());
}

UINT CNetworkInfoDlg::GetInternetIPThreadFunc(LPVOID lpParam)
{
	CCommon::SetThreadLanguage(theApp.m_general_data.language);		//设置线程语言
	CNetworkInfoDlg* p_instance = (CNetworkInfoDlg*)lpParam;
	p_instance->m_internet_ip_address = CCommon::GetInternetIp();
	if (!p_instance->m_internet_ip_address.empty())
		p_instance->m_info_list.SetItemText(14, 1, p_instance->m_internet_ip_address.c_str());
	else
		p_instance->m_info_list.SetItemText(14, 1, CCommon::LoadText(IDS_GET_FAILED));
	return 0;
}

void CNetworkInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO_LIST1, m_info_list);
}


BEGIN_MESSAGE_MAP(CNetworkInfoDlg, CDialog)
	ON_COMMAND(ID_COPY_TEXT, &CNetworkInfoDlg::OnCopyText)
	ON_NOTIFY(NM_RCLICK, IDC_INFO_LIST1, &CNetworkInfoDlg::OnNMRClickInfoList1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CNetworkInfoDlg 消息处理程序


BOOL CNetworkInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(CCommon::LoadText(IDS_TITLE_CONNECTION_DETIAL));

	//获取IP地址
	GetIPAddress();

	//初始化列表控件
	CRect rect;
	m_info_list.GetClientRect(rect);
	m_info_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
	int width0, width1;
	width0 = rect.Width() / 4;
	width1 = rect.Width() - width0 - theApp.DPI(21);
	m_info_list.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, width0);		//插入第0列
	m_info_list.InsertColumn(1, CCommon::LoadText(IDS_VALUE), LVCFMT_LEFT, width1);		//插入第1列

	//显示列表中的信息
	ShowInfo();

	//CCommon::GetInternetIp();
	m_pGetIPThread = AfxBeginThread(GetInternetIPThreadFunc, this);		//启动获取外网IP的线程

	//SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//取消置顶
	m_info_list.GetToolTips()->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	m_menu.LoadMenu(IDR_INFO_MENU); //装载右键菜单

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CNetworkInfoDlg::OnCopyText()
{
	// TODO: 在此添加命令处理程序代码
	if (!CCommon::CopyStringToClipboard(wstring(m_selected_string)))
		MessageBox(CCommon::LoadText(IDS_COPY_TO_CLIPBOARD_FAILED), NULL, MB_ICONWARNING);
}


void CNetworkInfoDlg::OnNMRClickInfoList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	//获取鼠标点击处的文本
	int item, sub_item;
	item = pNMItemActivate->iItem;
	sub_item = pNMItemActivate->iSubItem;
	m_selected_string = m_info_list.GetItemText(item, sub_item);

	//弹出右键菜单
	CMenu* pContextMenu = m_menu.GetSubMenu(0);	//获取第一个弹出菜单
	CPoint point1;	//定义一个用于确定光标位置的位置  
	GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //在指定位置显示弹出菜单

	*pResult = 0;
}


void CNetworkInfoDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//对话框关闭时强制结束获取IP地址的线程
	TerminateThread(m_pGetIPThread->m_hThread, 0);

	CDialog::OnClose();
}
