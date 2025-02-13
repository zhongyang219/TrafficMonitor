// HistoryTrafficList.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "HistoryTrafficListDlg.h"
#include "afxdialogex.h"
#include "CalendarHelper.h"


// CHistoryTrafficListDlgDlg 对话框

IMPLEMENT_DYNAMIC(CHistoryTrafficListDlg, CTabDlg)

CHistoryTrafficListDlg::CHistoryTrafficListDlg(deque<HistoryTraffic>& history_traffics, CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_HISTORY_TRAFFIC_LIST_DIALOG, pParent), m_history_traffics(history_traffics)
{

}

CHistoryTrafficListDlg::~CHistoryTrafficListDlg()
{
}


bool CHistoryTrafficListDlg::CalculateColumeWidth(std::vector<int>& widths)
{
    if (!IsWindow(m_history_list.GetSafeHwnd()))
        return false;
    CRect rect;
    m_history_list.GetWindowRect(rect);
    if (rect.Width() <= 0)
        return false;
    const int MAX_COLUME{ 5 };
	int width_date;		//“日期”列的宽度
    int width0;			//“上传”、“下载”、“总流量”列的宽度
    int width1;			//“图表”列的宽度
	width_date = rect.Width() * 3 / 11;
    width0 = rect.Width() * 2 / 11;
    if (width_date > theApp.DPI(150))
		width_date = theApp.DPI(150);
	if (width0 > theApp.DPI(120))
		width0 = theApp.DPI(120);
    width1 = rect.Width() - (MAX_COLUME - 2) * width0 - width_date - theApp.DPI(20) - 1;
    widths.resize(MAX_COLUME);
	widths[0] = width_date;
    widths[1] = widths[2] = widths[3] = width0;
    widths[4] = width1;
    return true;
}

void CHistoryTrafficListDlg::AddListRow(const ListRowData& data, unsigned __int64 max_traffic)
{
    unsigned __int64 total_kbytes = data.up_kBytes + data.down_kBytes;
    int index = m_history_list.GetItemCount();
    m_history_list.InsertItem(index, data.str);
    if (data.mixed)
    {
        m_history_list.SetItemText(index, 1, _T("-"));
        m_history_list.SetItemText(index, 2, _T("-"));
    }
    else
    {
        m_history_list.SetItemText(index, 1, CCommon::KBytesToString(data.up_kBytes));
        m_history_list.SetItemText(index, 2, CCommon::KBytesToString(data.down_kBytes));
    }
    m_history_list.SetItemText(index, 3, CCommon::KBytesToString(total_kbytes));

    double range = static_cast<double>(total_kbytes) * 1000 / max_traffic;
    COLORREF color;
    if (total_kbytes < 1024 * 1024)		//流量小于1GB时绘制蓝色
        color = TRAFFIC_COLOR_BLUE;
    else if (total_kbytes < 10 * 1024 * 1024)	//流量小于10GB时绘制绿色
        color = TRAFFIC_COLOR_GREEN;
    else if (total_kbytes < 100 * 1024 * 1024)	//流量小于100GB时绘制黄色
        color = TRAFFIC_COLOR_YELLOE;
    else if (total_kbytes < 1024 * 1024 * 1024)	//流量小于1TB时绘制红色
        color = TRAFFIC_COLOR_RED;
    else		//流量超过1TB时显示深红色
        color = TRAFFIC_COLOR_DARK_RED;
    m_history_list.SetDrawItemRangeData(index, range, color);
}

void CHistoryTrafficListDlg::ShowListData()
{
    m_history_list.DeleteAllItems();

    //显示日视图
    if(theApp.m_cfg_data.m_view_type == HistoryTrafficViewType::HV_DAY)
    {
        //获取历史流量列表中流量的最大值
        unsigned __int64 max_traffic{};
        for (const auto& traffic : m_history_traffics)
        {
            if (traffic.kBytes() > max_traffic)
                max_traffic = traffic.kBytes();
        }

        for (size_t i{}; i < m_history_traffics.size(); i++)
        {
            CString date_str;
            //CString k_bytes_str;
            date_str.Format(_T("%.4d/%.2d/%.2d ("), m_history_traffics[i].year, m_history_traffics[i].month, m_history_traffics[i].day);

            int week_day = CCalendarHelper::CaculateWeekDay(m_history_traffics[i].year, m_history_traffics[i].month, m_history_traffics[i].day);
            switch (week_day)
            {
            case 0:
                date_str += CCommon::LoadText(IDS_SUNDAY);
                break;
            case 1:
                date_str += CCommon::LoadText(IDS_MONDAY);
                break;
            case 2:
                date_str += CCommon::LoadText(IDS_TUESDAY);
                break;
            case 3:
                date_str += CCommon::LoadText(IDS_WEDNESDAY);
                break;
            case 4:
                date_str += CCommon::LoadText(IDS_THURSDAY);
                break;
            case 5:
                date_str += CCommon::LoadText(IDS_FRIDAY);
                break;
            case 6:
                date_str += CCommon::LoadText(IDS_SATURDAY);
                break;
            default:
                break;
            }
            date_str += _T(')');

            ListRowData data;
            data.str = date_str;
            data.up_kBytes = m_history_traffics[i].up_kBytes;
            data.down_kBytes = m_history_traffics[i].down_kBytes;
            data.mixed = m_history_traffics[i].mixed;
            AddListRow(data, max_traffic);
        }
    }

    //显示月/季/年视图
    else
    {
        std::vector<ListRowData> list_data;
        unsigned __int64 max_traffic{};
        for (const auto& traffic : m_history_traffics)
        {
            CString date_str;
            if (theApp.m_cfg_data.m_view_type == HistoryTrafficViewType::HV_WEEK)
            {
                date_str.Format(_T("%.4d/"), traffic.year);
                date_str += CCommon::LoadTextFormat(IDS_WEEK_NUM, { traffic.week() });
            }
            else if (theApp.m_cfg_data.m_view_type == HistoryTrafficViewType::HV_MONTH)      //月视图
            {
                date_str.Format(_T("%.4d/%.2d"), traffic.year, traffic.month);
            }
            else if (theApp.m_cfg_data.m_view_type == HistoryTrafficViewType::HV_QUARTER)   //季视图
            {
                date_str.Format(_T("%.4d/"), traffic.year);
                if (traffic.month <= 3)
                    date_str += _T("Q1");
                else if (traffic.month <= 6)
                    date_str += _T("Q2");
                else if (traffic.month <= 9)
                    date_str += _T("Q3");
                else
                    date_str += _T("Q4");
            }
            else    //年视图
            {
                date_str.Format(_T("%.4d"), traffic.year);
            }
            if (list_data.empty() || list_data.back().str != date_str)
            {
                if(!list_data.empty())
                {
                    unsigned __int64 cur_traffic{ list_data.back().up_kBytes + list_data.back().down_kBytes };
                    if (cur_traffic > max_traffic)
                        max_traffic = cur_traffic;
                }
                ListRowData data;
                data.str = date_str;
                data.up_kBytes = traffic.up_kBytes;
                data.down_kBytes = traffic.down_kBytes;
                list_data.push_back(data);
            }
            else
            {
                list_data.back().up_kBytes += traffic.up_kBytes;
                list_data.back().down_kBytes += traffic.down_kBytes;
            }
        }

        for (const auto& data : list_data)
        {
            AddListRow(data, max_traffic);
        }
    }
}

void CHistoryTrafficListDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_HISTORY_INFO_LIST, m_history_list);
    DDX_Control(pDX, IDC_VIEW_TYPE_COMBO, m_view_type_combo);
    DDX_Control(pDX, IDC_VIEW_SCALE_COMBO, m_view_scale_combo);
}


BEGIN_MESSAGE_MAP(CHistoryTrafficListDlg, CTabDlg)
	ON_WM_INITMENU()
	ON_COMMAND(ID_USE_LINEAR_SCALE, &CHistoryTrafficListDlg::OnUseLinearScale)
	ON_COMMAND(ID_USE_LOG_SCALE, &CHistoryTrafficListDlg::OnUseLogScale)
    ON_WM_SIZE()
    ON_CBN_SELCHANGE(IDC_VIEW_TYPE_COMBO, &CHistoryTrafficListDlg::OnCbnSelchangeViewTypeCombo)
    ON_CBN_SELCHANGE(IDC_VIEW_SCALE_COMBO, &CHistoryTrafficListDlg::OnCbnSelchangeViewScaleCombo)
END_MESSAGE_MAP()


// CHistoryTrafficListDlgDlg 消息处理程序


BOOL CHistoryTrafficListDlg::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//初始化列表控件
	m_history_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    std::vector<int> widths;
    CalculateColumeWidth(widths);
    m_history_list.InsertColumn(0, CCommon::LoadText(IDS_DATE), LVCFMT_LEFT, widths[0]);
	m_history_list.InsertColumn(1, CCommon::LoadText(IDS_UPLOAD), LVCFMT_RIGHT, widths[1]);
	m_history_list.InsertColumn(2, CCommon::LoadText(IDS_DOWNLOAD), LVCFMT_RIGHT, widths[2]);
	m_history_list.InsertColumn(3, CCommon::LoadText(IDS_TRAFFIC_USED), LVCFMT_RIGHT, widths[3]);
	m_history_list.InsertColumn(4, CCommon::LoadText(IDS_FIGURE), LVCFMT_LEFT, widths[4]);

    m_history_list.EnableDrawItemRange();
    m_history_list.SetDrawItemRangeRow(4);
    m_history_list.SetDrawItemRangMargin(theApp.DPI(4));
    m_history_list.SetDrawItemRangInLogScale(theApp.m_cfg_data.m_use_log_scale);

    //初始化控件
    m_view_type_combo.AddString(CCommon::LoadText(IDS_DAY_VIEW));
    m_view_type_combo.AddString(CCommon::LoadText(IDS_WEEK_VIEW));
    m_view_type_combo.AddString(CCommon::LoadText(IDS_MONTH_VIEW));
    m_view_type_combo.AddString(CCommon::LoadText(IDS_QUARTER_VIEW));
    m_view_type_combo.AddString(CCommon::LoadText(IDS_YEAR_VIEW));
    m_view_type_combo.SetCurSel(static_cast<int>(theApp.m_cfg_data.m_view_type));

    m_view_scale_combo.AddString(CCommon::LoadText(IDS_LINEAR_SCALE));
    m_view_scale_combo.AddString(CCommon::LoadText(IDS_LOG_SCALE));
    m_view_scale_combo.SetCurSel(theApp.m_cfg_data.m_use_log_scale ? 1 : 0);

    //显示列表数据
    ShowListData();

    CCommon::LoadMenuResource(m_menu, IDR_HISTORY_TRAFFIC_MENU);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CHistoryTrafficListDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类

	LPNMHDR lpnmhdr = (LPNMHDR)lParam;
	if (lpnmhdr->code == NM_RCLICK)
	{
		CPoint point;
		GetCursorPos(&point);//获得光标的位置
		m_history_list.ScreenToClient(&point);//获得list控件在窗口上的坐标
		CWnd* pWnd = m_history_list.ChildWindowFromPoint(point);
		CHeaderCtrl* pHeader = m_history_list.GetHeaderCtrl();//获取列表视图控件的标题控件
		CRect item_rect;
		pHeader->GetItemRect(4, item_rect);		//获取列表标题控件第2列的矩形区域，只有光标在第4列点击时才弹出右键菜单
		if (pWnd && (pWnd->GetSafeHwnd() == pHeader->GetSafeHwnd()) && item_rect.PtInRect(point))
		{
			HDHITTESTINFO info{};
			info.pt = point;
			pHeader->SendMessage(HDM_HITTEST, 0, (LPARAM)&info);
			CMenu * pMenu = m_menu.GetSubMenu(0);
			GetCursorPos(&point);
			pMenu->TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, this);
		}
	}
	return CTabDlg::OnNotify(wParam, lParam, pResult);
}


void CHistoryTrafficListDlg::OnInitMenu(CMenu* pMenu)
{
	CTabDlg::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码
	if (theApp.m_cfg_data.m_use_log_scale)
		pMenu->CheckMenuRadioItem(ID_USE_LINEAR_SCALE, ID_USE_LOG_SCALE, ID_USE_LOG_SCALE, MF_BYCOMMAND | MF_CHECKED);
	else
		pMenu->CheckMenuRadioItem(ID_USE_LINEAR_SCALE, ID_USE_LOG_SCALE, ID_USE_LINEAR_SCALE, MF_BYCOMMAND | MF_CHECKED);
}


void CHistoryTrafficListDlg::OnUseLinearScale()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_cfg_data.m_use_log_scale = false;
	m_history_list.SetDrawItemRangInLogScale(theApp.m_cfg_data.m_use_log_scale);
}


void CHistoryTrafficListDlg::OnUseLogScale()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_cfg_data.m_use_log_scale = true;
	m_history_list.SetDrawItemRangInLogScale(theApp.m_cfg_data.m_use_log_scale);
}


void CHistoryTrafficListDlg::OnSize(UINT nType, int cx, int cy)
{
    CTabDlg::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (nType != SIZE_MINIMIZED)
    {
        std::vector<int> widths;
        if(CalculateColumeWidth(widths))
        {
            for (size_t i{}; i < widths.size(); i++)
            {
                m_history_list.SetColumnWidth(i, widths[i]);
            }
        }
    }
}


void CHistoryTrafficListDlg::OnCbnSelchangeViewTypeCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    theApp.m_cfg_data.m_view_type = static_cast<HistoryTrafficViewType>(m_view_type_combo.GetCurSel());
    ShowListData();
}


void CHistoryTrafficListDlg::OnCbnSelchangeViewScaleCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    theApp.m_cfg_data.m_use_log_scale = (m_view_scale_combo.GetCurSel() != 0);
    m_history_list.SetDrawItemRangInLogScale(theApp.m_cfg_data.m_use_log_scale);
}
