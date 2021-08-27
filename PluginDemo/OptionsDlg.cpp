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
END_MESSAGE_MAP()


// COptionsDlg 消息处理程序
