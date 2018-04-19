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
	m_info_list.InsertItem(0, _T("接口名"));
	m_info_list.SetItemText(0, 1, m_network_info.wszName);

	m_info_list.InsertItem(1, _T("接口描述"));
	m_info_list.SetItemText(1, 1, CCommon::StrToUnicode((const char*)m_network_info.bDescr).c_str());

	m_info_list.InsertItem(2, _T("连接类型"));
	switch (m_network_info.dwType)
	{
	case IF_TYPE_OTHER: temp = _T("其他类型网络"); break;
	case IF_TYPE_ETHERNET_CSMACD: temp = _T("以太网网络"); break;
	case IF_TYPE_ISO88025_TOKENRING: temp = _T("令牌环网络"); break;
	case IF_TYPE_FDDI: temp = _T("光纤分布式数据接口 (FDDI) 网络"); break;
	case IF_TYPE_PPP: temp = _T("PPP 网络"); break;
	case IF_TYPE_SOFTWARE_LOOPBACK: temp = _T("软件环回网络"); break;
	case IF_TYPE_ATM: temp = _T("ATM 网络"); break;
	case IF_TYPE_IEEE80211: temp = _T("IEEE 802.11 无线网络"); break;
	case IF_TYPE_TUNNEL: temp = _T("隧道类型封装网络"); break;
	case IF_TYPE_IEEE1394: temp = _T("IEEE 1394 (Firewire) 高性能串行总线网络"); break;
	case IF_TYPE_IEEE80216_WMAN: temp = _T("对于 WiMax 设备的移动宽带"); break;
	case IF_TYPE_WWANPP: temp = _T("基于 GSM 网络设备的移动宽带"); break;
	case IF_TYPE_WWANPP2: temp = _T("基于 CDMA 的设备移动宽带"); break;
	default: temp = _T("未知网络"); break;
	}
	m_info_list.SetItemText(2, 1, temp);

	//m_info_list.InsertItem(3, _T("最大传输单位大小"));
	//temp.Format(_T("%u"), m_network_info.dwMtu);
	//m_info_list.SetItemText(3, 1, temp);

	m_info_list.InsertItem(3, _T("速度"));
	temp.Format(_T("%dMbps"), m_network_info.dwSpeed / 1000000);
	m_info_list.SetItemText(3, 1, temp);

	m_info_list.InsertItem(4, _T("适配器物理地址"));
	temp = _T("");
	char buff[3];
	for (int i{}; i < m_network_info.dwPhysAddrLen; i++)
	{
		//_itoa_s(m_network_info.bPhysAddr[i], buff, 16);
		sprintf_s(buff, "%.2x", m_network_info.bPhysAddr[i]);
		temp += buff;
		if (i != m_network_info.dwPhysAddrLen - 1)
			temp += _T('-');
	}
	m_info_list.SetItemText(4, 1, temp);

	m_info_list.InsertItem(5, _T("IP地址"));
	m_info_list.SetItemText(5, 1, m_ip_address.c_str());

	m_info_list.InsertItem(6, _T("子网掩码"));
	m_info_list.SetItemText(6, 1, m_subnet_mask.c_str());

	m_info_list.InsertItem(7, _T("默认网关"));
	m_info_list.SetItemText(7, 1, m_default_gateway.c_str());

	////temp.Format(_T("物理地址长度：%d\r\n"), m_network_info.dwPhysAddrLen);
	////out_info += temp;
	////temp = _T("适配器物理地址：");
	////temp += StrToUnicode((const char*)m_network_info.bPhysAddr).c_str();
	////out_info += temp;
	////out_info += _T("\r\n");

	//m_info_list.InsertItem(5, _T("管理员状态"));
	//m_info_list.SetItemText(5, 1, m_network_info.dwAdminStatus ? _T("启用") : _T("禁用"));

	m_info_list.InsertItem(8, _T("连接状态"));
	switch (m_network_info.dwOperStatus)
	{
	case IF_OPER_STATUS_NON_OPERATIONAL: temp = _T("LAN 适配器已被禁用"); break;
	case IF_OPER_STATUS_UNREACHABLE: temp = _T("WAN 适配器未连接"); break;
	case IF_OPER_STATUS_DISCONNECTED: temp = _T("网络电缆断开连接或无载体"); break;
	case IF_OPER_STATUS_CONNECTING: temp = _T("WAN 适配器正在连接"); break;
	case IF_OPER_STATUS_CONNECTED: temp = _T("WAN 适配器连接到远程对等方"); break;
	case IF_OPER_STATUS_OPERATIONAL: temp = _T("LAN 适配器已连接"); break;
	default: temp = _T("未知状态"); break;
	}
	m_info_list.SetItemText(8, 1, temp);

	m_info_list.InsertItem(9, _T("已接收字节数"));
	temp.Format(_T("%u (%s)"), m_network_info.dwInOctets, CCommon::DataSizeToString(m_network_info.dwInOctets));
	m_info_list.SetItemText(9, 1, temp);

	m_info_list.InsertItem(10, _T("已发送字节数"));
	temp.Format(_T("%u (%s)"), m_network_info.dwOutOctets, CCommon::DataSizeToString(m_network_info.dwOutOctets));
	m_info_list.SetItemText(10, 1, temp);

	m_info_list.InsertItem(11, _T("自程序启动以来已接收字节数"));
	temp.Format(_T("%u (%s)"), m_in_bytes, CCommon::DataSizeToString(m_in_bytes));
	m_info_list.SetItemText(11, 1, temp);

	m_info_list.InsertItem(12, _T("自程序启动以来已发送字节数"));
	temp.Format(_T("%u (%s)"), m_out_bytes, CCommon::DataSizeToString(m_out_bytes));
	m_info_list.SetItemText(12, 1, temp);

	m_info_list.InsertItem(13, _T("程序已运行时间"));
	SYSTEMTIME current_time, time;
	GetLocalTime(&current_time);
	time = CCommon::CompareSystemTime(current_time, m_start_time);
	temp.Format(_T("%d小时%d分%d秒"), time.wHour, time.wMinute, time.wSecond);
	m_info_list.SetItemText(13, 1, temp);

	m_info_list.InsertItem(14, _T("外网IP地址"));
	m_info_list.SetItemText(14, 1, m_internet_ip_address.c_str());
}

UINT CNetworkInfoDlg::GetInternetIPThreadFunc(LPVOID lpParam)
{
	CNetworkInfoDlg* p_instance = (CNetworkInfoDlg*)lpParam;
	p_instance->m_internet_ip_address = CCommon::GetInternetIp();
	if (!p_instance->m_internet_ip_address.empty())
		p_instance->m_info_list.SetItemText(14, 1, p_instance->m_internet_ip_address.c_str());
	else
		p_instance->m_info_list.SetItemText(14, 1, _T("获取失败"));
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

	//获取IP地址
	GetIPAddress();

	//初始化列表控件
	CRect rect;
	m_info_list.GetClientRect(rect);
	m_info_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
	int width0, width1;
	width0 = rect.Width() / 4;
	width1 = rect.Width() - width0 - theApp.DPI(21);
	m_info_list.InsertColumn(0, _T("项目"), LVCFMT_LEFT, width0);		//插入第0列
	m_info_list.InsertColumn(1, _T("值"), LVCFMT_LEFT, width1);		//插入第1列

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
		MessageBox(_T("复制到剪贴板失败！"), NULL, MB_ICONWARNING);
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
