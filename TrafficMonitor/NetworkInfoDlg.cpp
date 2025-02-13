// CNetworkInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "NetworkInfoDlg.h"
#include "afxdialogex.h"


// CNetworkInfoDlg 对话框

IMPLEMENT_DYNAMIC(CNetworkInfoDlg, CBaseDialog)

CNetworkInfoDlg::CNetworkInfoDlg(vector<NetWorkConection>& adapters, MIB_IFROW* pIfRow, int connection_selected, CWnd* pParent /*=NULL*/)
    : CBaseDialog(IDD_NETWORK_INFO_DIALOG, pParent), m_connections(adapters), m_pIfRow(pIfRow), m_connection_selected(connection_selected)
{
    m_current_connection = connection_selected;
}

CNetworkInfoDlg::~CNetworkInfoDlg()
{
}


void CNetworkInfoDlg::ShowInfo()
{
    CString temp;
    MIB_IFROW& network_info = GetConnectIfTable(m_connection_selected);
    //接口名
    m_info_list.SetItemText(0, 1, network_info.wszName);
    //接口描述
    m_info_list.SetItemText(1, 1, CCommon::StrToUnicode((const char*)network_info.bDescr).c_str());
    //连接类型
    switch (network_info.dwType)
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
    //速度
    temp.Format(_T("%dMbps"), network_info.dwSpeed / 1000000);
    m_info_list.SetItemText(3, 1, temp);
    //适配器物理地址
    temp = _T("");
    char buff[3];
    for (size_t i{}; i < network_info.dwPhysAddrLen; i++)
    {
        sprintf_s(buff, "%.2x", network_info.bPhysAddr[i]);
        temp += buff;
        if (i != network_info.dwPhysAddrLen - 1)
            temp += _T('-');
    }
    m_info_list.SetItemText(4, 1, temp);
    //IP地址
    m_info_list.SetItemText(5, 1, GetConnection(m_connection_selected).ip_address.c_str());
    //子网掩码
    m_info_list.SetItemText(6, 1, GetConnection(m_connection_selected).subnet_mask.c_str());
    //默认网关
    m_info_list.SetItemText(7, 1, GetConnection(m_connection_selected).default_gateway.c_str());
    //连接状态
    switch (network_info.dwOperStatus)
    {
    case IF_OPER_STATUS_NON_OPERATIONAL: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_NON_OPERATIONAL); break;
    case IF_OPER_STATUS_UNREACHABLE: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_UNREACHABLE); break;
    case IF_OPER_STATUS_DISCONNECTED: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_DISCONNECTED); break;
    case IF_OPER_STATUS_CONNECTING: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_CONNECTING); break;
    case IF_OPER_STATUS_CONNECTED: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_CONNECTED); break;
    case IF_OPER_STATUS_OPERATIONAL: temp = CCommon::LoadText(IDS_IF_OPER_STATUS_OPERATIONAL); break;
        //case IfOperStatusUp:
        //    temp = CCommon::LoadText(IDS_IF_OPER_STATUS_UP);
        //    break;
        //case IfOperStatusDown:
        //case IfOperStatusNotPresent:
        //case IfOperStatusLowerLayerDown:
        //    temp = CCommon::LoadText(IDS_IF_OPER_STATUS_DOWN);
        //    break;
        //case IfOperStatusTesting:
        //case IfOperStatusUnknown:
        //    temp = CCommon::LoadText(IDS_UNKNOW_STATUS);
        //    break;
        //case IfOperStatusDormant:
        //    temp = CCommon::LoadText(IDS_IF_OPER_STATUS_DORMANT);
            //break;
    default: temp = CCommon::LoadText(IDS_UNKNOW_STATUS); break;
    }
    m_info_list.SetItemText(8, 1, temp);
    //已接收字节数
    temp.Format(_T("%s (%s)"), CCommon::IntToString(network_info.dwInOctets, true, true), CCommon::DataSizeToString(network_info.dwInOctets));
    m_info_list.SetItemText(9, 1, temp);
    //已发送字节数
    temp.Format(_T("%s (%s)"), CCommon::IntToString(network_info.dwOutOctets, true, true), CCommon::DataSizeToString(network_info.dwOutOctets));
    m_info_list.SetItemText(10, 1, temp);
    //自程序启动以来已接收字节数
    unsigned __int64 in_bytes_since_start;
    in_bytes_since_start = network_info.dwInOctets - GetConnection(m_connection_selected).in_bytes;
    temp.Format(_T("%s (%s)"), CCommon::IntToString(in_bytes_since_start, true, true), CCommon::DataSizeToString(in_bytes_since_start));
    m_info_list.SetItemText(11, 1, temp);
    //自程序启动以来已发送字节数
    unsigned __int64 out_bytes_since_start;
    out_bytes_since_start = network_info.dwOutOctets - GetConnection(m_connection_selected).out_bytes;
    temp.Format(_T("%s (%s)"), CCommon::IntToString(out_bytes_since_start, true, true), CCommon::DataSizeToString(out_bytes_since_start));
    m_info_list.SetItemText(12, 1, temp);

    //显示当前选择指示
    CString str;
    str.Format(_T("%d/%d"), m_connection_selected + 1, m_connections.size());
    SetDlgItemText(IDC_INDEX_STATIC, str);
    CFont* font = GetFont();
    CWnd* index_static = GetDlgItem(IDC_INDEX_STATIC);
    if (m_current_connection == m_connection_selected && !theApp.m_cfg_data.m_select_all)
        index_static->SetFont(&m_font_bold);
    else
        index_static->SetFont(font);
}

void CNetworkInfoDlg::GetProgramElapsedTime()
{
    //程序已运行时间
    SYSTEMTIME current_time, time;
    GetLocalTime(&current_time);
    time = CCommon::CompareSystemTime(current_time, m_start_time);
    CString temp;
    temp.Format(CCommon::LoadText(IDS_HOUR_MINUTE_SECOND), time.wHour, time.wMinute, time.wSecond);
    m_info_list.SetItemText(13, 1, temp);
}

MIB_IFROW& CNetworkInfoDlg::GetConnectIfTable(int connection_index)
{
    static MIB_IFROW nouse{};
    if (connection_index >= 0 && connection_index < static_cast<int>(m_connections.size()))
    {
        int index = m_connections[connection_index].index;
        if (m_pIfRow != nullptr)
            return m_pIfRow[index];
    }
    return nouse;
}

NetWorkConection CNetworkInfoDlg::GetConnection(int connection_index)
{
    if (connection_index >= 0 && connection_index < static_cast<int>(m_connections.size()))
        return m_connections[connection_index];
    else
        return NetWorkConection();
}

UINT CNetworkInfoDlg::GetInternetIPThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguage(theApp.m_general_data.language);		//设置线程语言
    CNetworkInfoDlg* p_instance = (CNetworkInfoDlg*)lpParam;
    wstring ip_address, ip_location;

    //IPV4
    CCommon::GetInternetIp2(ip_address, ip_location, false);			//获取外网IP地址，
    if (!IsWindow(p_instance->GetSafeHwnd()))		//如果当前对话框已经销毁，则退出线程
        return 0;
    if (!ip_address.empty())
    {
        CString info;
        if (ip_location.empty())
            info = ip_address.c_str();
        else
            info.Format(_T("%s (%s)"), ip_address.c_str(), ip_location.c_str());
        p_instance->m_info_list.SetItemText(14, 1, info);
    }
    else
    {
        p_instance->m_info_list.SetItemText(14, 1, CCommon::LoadText(IDS_GET_FAILED));
    }

    //IPV6
    wstring ipv6_address, ipv6_location;
    CCommon::GetInternetIp2(ip_address, ip_location, true);			//获取外网IP地址，
    if (!IsWindow(p_instance->GetSafeHwnd()))		//如果当前对话框已经销毁，则退出线程
        return 0;
    if (!ip_address.empty())
    {
        CString info;
        if (ip_location.empty())
            info = ip_address.c_str();
        else
            info.Format(_T("%s (%s)"), ip_address.c_str(), ip_location.c_str());
        p_instance->m_info_list.SetItemText(15, 1, info);
    }
    else
    {
        p_instance->m_info_list.SetItemText(15, 1, CCommon::LoadText(IDS_GET_FAILED));
    }

    p_instance->m_ip_acquired = true;
    return 0;
}

CString CNetworkInfoDlg::GetDialogName() const
{
    return _T("NetworkInfoDlg");
}

void CNetworkInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_INFO_LIST1, m_info_list);
}


BEGIN_MESSAGE_MAP(CNetworkInfoDlg, CBaseDialog)
    ON_COMMAND(ID_COPY_TEXT, &CNetworkInfoDlg::OnCopyText)
    ON_NOTIFY(NM_RCLICK, IDC_INFO_LIST1, &CNetworkInfoDlg::OnNMRClickInfoList1)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_PREVIOUS_BUTTON, &CNetworkInfoDlg::OnBnClickedPreviousButton)
    ON_BN_CLICKED(IDC_NEXT_BUTTON, &CNetworkInfoDlg::OnBnClickedNextButton)
    ON_WM_TIMER()
    ON_WM_MOUSEWHEEL()
    ON_NOTIFY(NM_DBLCLK, IDC_INFO_LIST1, &CNetworkInfoDlg::OnNMDblclkInfoList1)
END_MESSAGE_MAP()


// CNetworkInfoDlg 消息处理程序


BOOL CNetworkInfoDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(theApp.GetMenuIcon(IDI_INFO), FALSE);		// 设置小图标

    //重新获取IP地址
    CAdapterCommon::RefreshIpAddress(m_connections);

    //初始化列表控件
    CRect rect;
    m_info_list.GetClientRect(rect);
    m_info_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    int width0, width1;
    width0 = rect.Width() / 4;
    width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_info_list.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, width0);		//插入第0列
    m_info_list.InsertColumn(1, CCommon::LoadText(IDS_VALUE), LVCFMT_LEFT, width1);		//插入第1列

    //向列表中插入行
    m_info_list.InsertItem(0, CCommon::LoadText(IDS_INTERFACE_NAME));
    m_info_list.InsertItem(1, CCommon::LoadText(IDS_INTERFACE_DESCRIPTION));
    m_info_list.InsertItem(2, CCommon::LoadText(IDS_CONNECTION_TYPE));
    m_info_list.InsertItem(3, CCommon::LoadText(IDS_SPEED));
    m_info_list.InsertItem(4, CCommon::LoadText(IDS_ADAPTER_PHYSICAL_ADDRESS));
    m_info_list.InsertItem(5, CCommon::LoadText(IDS_IP_ADDRESS));
    m_info_list.InsertItem(6, CCommon::LoadText(IDS_SUBNET_MASK));
    m_info_list.InsertItem(7, CCommon::LoadText(IDS_DEFAULT_GATEWAY));
    m_info_list.InsertItem(8, CCommon::LoadText(IDS_OPERATIONAL_STATUS));
    m_info_list.InsertItem(9, CCommon::LoadText(IDS_BYTES_RECEIVED));
    m_info_list.InsertItem(10, CCommon::LoadText(IDS_BYTES_SENT));
    m_info_list.InsertItem(11, CCommon::LoadText(IDS_BYTES_RECEIVED_SINCE_START));
    m_info_list.InsertItem(12, CCommon::LoadText(IDS_BYTES_SENT_SINCE_START));
    m_info_list.InsertItem(13, CCommon::LoadText(IDS_PROGRAM_ELAPSED_TIME));
    m_info_list.InsertItem(14, CCommon::LoadText(IDS_INTERNET_IP_ADDRESS, _T(" (ipv4)")));
    m_info_list.InsertItem(15, CCommon::LoadText(IDS_INTERNET_IP_ADDRESS, _T(" (ipv6)")));
    //if (theApp.m_cfg_data.m_show_internet_ip)
    //{
    //	m_info_list.SetItemText(14, 1, CCommon::LoadText(IDS_ACQUIRING, _T("...")));
    //	m_info_list.SetItemText(15, 1, CCommon::LoadText(IDS_ACQUIRING, _T("...")));
    //}
    //else
    //{
    m_info_list.SetItemText(14, 1, CCommon::LoadText(IDS_DOUBLE_CLICK_TO_ACQUIRE));
    m_info_list.SetItemText(15, 1, CCommon::LoadText(IDS_DOUBLE_CLICK_TO_ACQUIRE));
    //}

    //显示列表中的信息
    LOGFONT lf{};
    GetFont()->GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    m_font_bold.CreateFontIndirect(&lf);
    ShowInfo();
    GetProgramElapsedTime();

    //CCommon::GetInternetIp();
    //if (theApp.m_cfg_data.m_show_internet_ip)
 //       m_pGetIPThread = AfxBeginThread(GetInternetIPThreadFunc, this);		//启动获取外网IP的线程

    //SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//取消置顶
    m_info_list.GetToolTips()->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    CCommon::LoadMenuResource(m_menu, IDR_INFO_MENU); //装载右键菜单

    SetTimer(CONNECTION_DETAIL_TIMER, 1000, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CNetworkInfoDlg::OnCopyText()
{
    // TODO: 在此添加命令处理程序代码
    if (!CCommon::CopyStringToClipboard(wstring(m_selected_string)))
        MessageBox(CCommon::LoadText(IDS_COPY_TO_CLIPBOARD_FAILED), NULL, MB_ICONWARNING);
}


void CNetworkInfoDlg::OnNMRClickInfoList1(NMHDR* pNMHDR, LRESULT* pResult)
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
    //if(theApp.m_cfg_data.m_show_internet_ip)
 //       TerminateThread(m_pGetIPThread->m_hThread, 0);
    CBaseDialog::OnClose();
}


void CNetworkInfoDlg::OnBnClickedPreviousButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_connections.size() > 1 && m_connection_selected > 0)
    {
        m_connection_selected--;
        ShowInfo();
    }
}


void CNetworkInfoDlg::OnBnClickedNextButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_connections.size() > 1 && m_connection_selected < m_connections.size() - 1)
    {
        m_connection_selected++;
        ShowInfo();
    }
}


BOOL CNetworkInfoDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_LEFT)
        {
            OnBnClickedPreviousButton();
            return TRUE;
        }
        if (pMsg->wParam == VK_RIGHT)
        {
            OnBnClickedNextButton();
            return TRUE;
        }
    }

    return CBaseDialog::PreTranslateMessage(pMsg);
}



void CNetworkInfoDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == CONNECTION_DETAIL_TIMER)
    {
        GetProgramElapsedTime();
    }

    CBaseDialog::OnTimer(nIDEvent);
}

BOOL CNetworkInfoDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //通过鼠标滚轮翻页
    if (zDelta > 0)
    {
        OnBnClickedPreviousButton();
    }
    if (zDelta < 0)
    {
        OnBnClickedNextButton();
    }

    return CBaseDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CNetworkInfoDlg::OnNMDblclkInfoList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    if (/*!theApp.m_cfg_data.m_show_internet_ip && */!m_ip_acquired && (pNMItemActivate->iItem == 14 || pNMItemActivate->iItem == 15))		//双击了IP地址一行时
    {
        m_info_list.SetItemText(14, 1, CCommon::LoadText(IDS_ACQUIRING, _T("...")));
        m_info_list.SetItemText(15, 1, CCommon::LoadText(IDS_ACQUIRING, _T("...")));
        m_pGetIPThread = AfxBeginThread(GetInternetIPThreadFunc, this);
    }
    *pResult = 0;
}
