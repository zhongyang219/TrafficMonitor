// BaseDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "BaseDialog.h"
#include "afxdialogex.h"
#include "IniHelper.h"
#include "TrafficMonitor.h"
#include "TrafficMonitorDlg.h"

// CBaseDialog 对话框
std::map<CString, HWND> CBaseDialog::m_unique_hwnd;

IMPLEMENT_DYNAMIC(CBaseDialog, CDialog)

CBaseDialog::CBaseDialog(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
    : CDialog(nIDTemplate, pParent)
{

}

CBaseDialog::~CBaseDialog()
{
}

void CBaseDialog::SetMinSize(int cx, int cy)
{
    m_min_size.cx = cx;
    m_min_size.cy = cy;
}

HWND CBaseDialog::GetUniqueHandel(LPCTSTR dlg_name)
{
    return m_unique_hwnd[dlg_name];
}

const std::map<CString, HWND>& CBaseDialog::AllUniqueHandels()
{
    return m_unique_hwnd;
}

bool CBaseDialog::IsAllDialogClosed()
{
    for (const auto& hwnd : m_unique_hwnd)
    {
        if (hwnd.second != nullptr)
            return false;
    }
    return true;
}

void CBaseDialog::LoadConfig()
{
    if (!GetDialogName().IsEmpty())
    {
        CIniHelper ini{ theApp.m_config_path };
        //载入窗口大小设置
        m_window_size.cx = ini.GetInt(_T("window_size"), GetDialogName() + _T("_width"), -1);
        m_window_size.cy = ini.GetInt(_T("window_size"), GetDialogName() + _T("_height"), -1);
    }
}

void CBaseDialog::SaveConfig() const
{
    if (!GetDialogName().IsEmpty())
    {
        CIniHelper ini{ theApp.m_config_path };
        //保存窗口大小设置
        ini.WriteInt(_T("window_size"), GetDialogName() + _T("_width"), m_window_size.cx);
        ini.WriteInt(_T("window_size"), GetDialogName() + _T("_height"), m_window_size.cy);
        ini.Save();
    }
}

void CBaseDialog::EnableDlgCtrl(UINT id, bool enable)
{
    CWnd* pWnd = GetDlgItem(id);
    if (pWnd != nullptr)
        pWnd->EnableWindow(enable);
}

void CBaseDialog::SetButtonIcon(UINT id, HICON hIcon)
{
    CWnd* dlgItem = GetDlgItem(id);
    CButton* btn = static_cast<CButton*>(dlgItem);
    if (btn != nullptr)
        btn->SetIcon(hIcon);
}

void CBaseDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBaseDialog, CDialog)
    ON_WM_DESTROY()
    ON_WM_GETMINMAXINFO()
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CBaseDialog 消息处理程序


BOOL CBaseDialog::OnInitDialog()
{
    m_unique_hwnd[GetDialogName()] = m_hWnd;
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //获取初始时窗口的大小
    if (m_min_size.cx <= 0 || m_min_size.cy <= 0)
    {
        CRect rect;
        GetWindowRect(rect);
        m_min_size.cx = rect.Width() * 96 / theApp.GetDpi();
        m_min_size.cy = rect.Height() * 96 / theApp.GetDpi();
    }

    //载入设置
    LoadConfig();

    //初始化窗口大小
    if (m_window_size.cx > 0 && m_window_size.cy > 0)
    {
        SetWindowPos(nullptr, 0, 0, m_window_size.cx, m_window_size.cy, SWP_NOZORDER | SWP_NOMOVE);
    }

    SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);       //取消置顶

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CBaseDialog::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    m_unique_hwnd[GetDialogName()] = NULL;
    SaveConfig();

    //当所有对话框关闭时重新设置主窗口置顶
    CTrafficMonitorDlg* pDlg = dynamic_cast<CTrafficMonitorDlg*>(theApp.m_pMainWnd);
    if (pDlg != nullptr && IsAllDialogClosed())
        pDlg->SetAlwaysOnTop();
}


void CBaseDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //限制窗口最小大小
    lpMMI->ptMinTrackSize.x = theApp.DPI(m_min_size.cx);		//设置最小宽度
    lpMMI->ptMinTrackSize.y = theApp.DPI(m_min_size.cy);		//设置最小高度

    CDialog::OnGetMinMaxInfo(lpMMI);
}


void CBaseDialog::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (nType != SIZE_MAXIMIZED && nType != SIZE_MINIMIZED)
    {
        //m_window_width = cx;
        //m_window_hight = cy;
        CRect rect;
        GetWindowRect(&rect);
        m_window_size.cx = rect.Width();
        m_window_size.cy = rect.Height();
    }

}


INT_PTR CBaseDialog::DoModal()
{
    HWND unique_hwnd{ m_unique_hwnd[GetDialogName()] };
    if (unique_hwnd != NULL && !GetDialogName().IsEmpty())      ///如果对话框已存在，则显示已存在的对话框
    {
        ::ShowWindow(unique_hwnd, SW_RESTORE);
        ::SetForegroundWindow(unique_hwnd);
        return 0;
    }
    return CDialog::DoModal();
}
