// DisplayTextSettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "DisplayTextSettingDlg.h"


// CDisplayTextSettingDlg 对话框

IMPLEMENT_DYNAMIC(CDisplayTextSettingDlg, CBaseDialog)

CDisplayTextSettingDlg::CDisplayTextSettingDlg(DispStrings& display_texts, CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_DISPLAY_TEXT_SETTINGS_DIALOG, pParent), m_display_texts(display_texts)
{

}

CDisplayTextSettingDlg::~CDisplayTextSettingDlg()
{
}

void CDisplayTextSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


CString CDisplayTextSettingDlg::GetDialogName() const
{
    return _T("DisplayTextSettingDlg");
}

BEGIN_MESSAGE_MAP(CDisplayTextSettingDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_RESTORE_DEFAULT_BUTTON, &CDisplayTextSettingDlg::OnBnClickedRestoreDefaultButton)
END_MESSAGE_MAP()


// CDisplayTextSettingDlg 消息处理程序


BOOL CDisplayTextSettingDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetIcon(theApp.GetMenuIcon(IDI_ITEM), FALSE);		// 设置小图标

    //初始化列表控件
    CRect rect;
    m_list_ctrl.GetClientRect(rect);
    m_list_ctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    int width0, width1;
    width0 = rect.Width() / 2;
    width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, width0);		//插入第0列
    m_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_VALUE), LVCFMT_LEFT, width1);		//插入第1列

    //向列表中插入行
    for (auto iter = m_display_texts.GetAllItems().begin(); iter != m_display_texts.GetAllItems().end(); ++iter)
    {
        CString item_name;
        switch (iter->first)
        {
        case TDI_UP:
            item_name = CCommon::LoadText(IDS_UPLOAD);
            break;
        case TDI_DOWN:
            item_name = CCommon::LoadText(IDS_DOWNLOAD);
            break;
        case TDI_CPU:
            item_name = _T("CPU");
            break;
        case TDI_MEMORY:
            item_name = CCommon::LoadText(IDS_MEMORY);
            break;
        case TDI_CPU_TEMP:
            item_name = CCommon::LoadText(IDS_CPU_TEMPERATURE);
            break;
        case TDI_GPU_TEMP:
            item_name = CCommon::LoadText(IDS_GPU_TEMPERATURE);
            break;
        case TDI_HDD_TEMP:
            item_name = CCommon::LoadText(IDS_HDD_TEMPERATURE);
            break;
        case TDI_MAIN_BOARD_TEMP:
            item_name = CCommon::LoadText(IDS_MAINBOARD_TEMPERATURE);
            break;
        default:
            break;
        }
        int index = m_list_ctrl.GetItemCount();
        m_list_ctrl.InsertItem(index, item_name);
        m_list_ctrl.SetItemText(index, 1, iter->second.c_str());
    }

    m_list_ctrl.SetEditColMethod(CListCtrlEx::EC_SPECIFIED);        //设置列表可编辑
    m_list_ctrl.SetEditableCol({ 1 });                              //设置可编辑的列

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CDisplayTextSettingDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    
    m_display_texts.Get(TDI_UP) = m_list_ctrl.GetItemText(0, 1).GetString();
    m_display_texts.Get(TDI_DOWN) = m_list_ctrl.GetItemText(1, 1).GetString();
    m_display_texts.Get(TDI_CPU) = m_list_ctrl.GetItemText(2, 1).GetString();
    m_display_texts.Get(TDI_MEMORY) = m_list_ctrl.GetItemText(3, 1).GetString();
    m_display_texts.Get(TDI_CPU_TEMP) = m_list_ctrl.GetItemText(4, 1).GetString();
    m_display_texts.Get(TDI_GPU_TEMP) = m_list_ctrl.GetItemText(5, 1).GetString();
    m_display_texts.Get(TDI_HDD_TEMP) = m_list_ctrl.GetItemText(6, 1).GetString();
    m_display_texts.Get(TDI_MAIN_BOARD_TEMP) = m_list_ctrl.GetItemText(7, 1).GetString();

    CBaseDialog::OnOK();
}



void CDisplayTextSettingDlg::OnBnClickedRestoreDefaultButton()
{
    // TODO: 在此添加控件通知处理程序代码
    m_list_ctrl.SetItemText(0, 1, _T("↑: "));
    m_list_ctrl.SetItemText(1, 1, _T("↓: "));
    m_list_ctrl.SetItemText(2, 1, _T("CPU: "));
    m_list_ctrl.SetItemText(3, 1, CCommon::LoadText(IDS_MEMORY_DISP, _T(": ")));
    m_list_ctrl.SetItemText(4, 1, _T("CPU: "));
    m_list_ctrl.SetItemText(5, 1, CCommon::LoadText(IDS_GPU_DISP, _T(": ")));
    m_list_ctrl.SetItemText(6, 1, CCommon::LoadText(IDS_HDD_DISP, _T(": ")));
    m_list_ctrl.SetItemText(7, 1, CCommon::LoadText(IDS_MAINBOARD_DISP, _T(": ")));
}
