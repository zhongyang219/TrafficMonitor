// OptionsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "OptionsDlg.h"
#include "afxdialogex.h"


// COptionsDlg 对话框

IMPLEMENT_DYNAMIC(COptionsDlg, CBaseDialog)

COptionsDlg::COptionsDlg(int tab, CWnd* pParent /*=NULL*/)
    : CBaseDialog(IDD_OPTIONS_DIALOG, pParent), m_tab_selected{ tab }
{

}

COptionsDlg::~COptionsDlg()
{
}

CString COptionsDlg::GetDialogName() const
{
    return OPTION_DLG_NAME;
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB1, m_tab);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CBaseDialog)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_APPLY_BUTTON, &COptionsDlg::OnBnClickedApplyButton)
END_MESSAGE_MAP()


// COptionsDlg 消息处理程序


BOOL COptionsDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetWindowText(CCommon::LoadText(IDS_TITLE_OPTION));
    SetIcon(theApp.GetMenuIcon(IDI_SETTINGS), FALSE);       // 设置小图标

    //创建子对话框
    m_tab1_dlg.Create(IDD_MAIN_WND_SETTINGS_DIALOG, &m_tab);
    m_tab2_dlg.Create(IDD_TASKBAR_SETTINGS_DIALOG, &m_tab);
    m_tab3_dlg.Create(IDD_GENERAL_SETTINGS_DIALOG, &m_tab);

    //保存子对话框
    m_tab_vect.push_back(&m_tab1_dlg);
    m_tab_vect.push_back(&m_tab2_dlg);
    m_tab_vect.push_back(&m_tab3_dlg);

    //获取子对话框的初始高度
    for (const auto* pDlg : m_tab_vect)
    {
        CRect rect;
        pDlg->GetWindowRect(rect);
        m_tab_height.push_back(rect.Height());
    }

    //添加对话框
    m_tab.AddWindow(&m_tab1_dlg, CCommon::LoadText(IDS_MAIN_WINDOW_SETTINGS));
    m_tab.AddWindow(&m_tab2_dlg, CCommon::LoadText(IDS_TASKBAR_WINDOW_SETTINGS));
    m_tab.AddWindow(&m_tab3_dlg, CCommon::LoadText(IDS_GENERAL_SETTINGS));

    //为每个标签添加图标
    CImageList ImageList;
    ImageList.Create(theApp.DPI(16), theApp.DPI(16), ILC_COLOR32 | ILC_MASK, 2, 2);
    ImageList.Add(theApp.GetMenuIcon(IDI_MAIN_WINDOW));
    ImageList.Add(theApp.GetMenuIcon(IDI_TASKBAR_WINDOW));
    ImageList.Add(theApp.GetMenuIcon(IDI_SETTINGS));
    m_tab.SetImageList(&ImageList);
    ImageList.Detach();

    m_tab.SetItemSize(CSize(theApp.DPI(60), theApp.DPI(24)));
    m_tab.AdjustTabWindowSize();

    //为每个子窗口设置滚动信息
    for (size_t i = 0; i < m_tab_vect.size(); i++)
    {
        m_tab_vect[i]->SetScrollbarInfo(m_tab.m_tab_rect.Height(), m_tab_height[i]);
    }

    //设置默认选中的标签
    if (m_tab_selected < 0 || m_tab_selected >= m_tab.GetItemCount())
        m_tab_selected = 0;
    m_tab.SetCurTab(m_tab_selected);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void COptionsDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_tab1_dlg.OnOK();
    m_tab2_dlg.OnOK();
    m_tab3_dlg.OnOK();

    CBaseDialog::OnOK();
}


void COptionsDlg::OnSize(UINT nType, int cx, int cy)
{
    CBaseDialog::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (nType != SIZE_MINIMIZED)
    {
        //为每个子窗口设置滚动信息
        for (size_t i = 0; i < m_tab_vect.size(); i++)
        {
            m_tab_vect[i]->ResetScroll();
            m_tab_vect[i]->SetScrollbarInfo(m_tab.m_tab_rect.Height(), m_tab_height[i]);
        }
    }
}


void COptionsDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_tab3_dlg.OnCancel();

    CBaseDialog::OnCancel();
}


void COptionsDlg::OnBnClickedApplyButton()
{
    m_tab2_dlg.SaveColorSettingToDefaultStyle();
    ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_SETTINGS_APPLIED, (WPARAM)this, 0);
    for (size_t i = 0; i < m_tab_vect.size(); i++)
    {
        m_tab_vect[i]->OnSettingsApplied();
    }
}
