// OptionsDlg.cpp: 实现文件
//

#include "pch.h"
#include "PluginDemo.h"
#include "OptionsDlg.h"
#include "afxdialogex.h"


// COptionsDlg 对话框

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)

COptionsDlg::COptionsDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_OPTIONS_DIALOG, pParent)
{

}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
    ON_BN_CLICKED(IDC_SHOW_SECOND_CHECK, &COptionsDlg::OnBnClickedShowSecondCheck)
END_MESSAGE_MAP()


// COptionsDlg 消息处理程序


BOOL COptionsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    //初始化控件状态
    CheckDlgButton(IDC_SHOW_SECOND_CHECK, m_data.show_second);
    //CheckDlgButton(IDC_SHOW_LABEL_CHECK, m_data.show_label_text);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void COptionsDlg::OnBnClickedShowSecondCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_second = (IsDlgButtonChecked(IDC_SHOW_SECOND_CHECK) != 0);
}
