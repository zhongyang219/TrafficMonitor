// SkinAutoAdaptSettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "afxdialogex.h"
#include "SkinAutoAdaptSettingDlg.h"


// CSkinAutoAdaptSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSkinAutoAdaptSettingDlg, CDialog)

CSkinAutoAdaptSettingDlg::CSkinAutoAdaptSettingDlg(const vector<wstring>& skins, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SKIN_AUTO_ADAPT_DLG, pParent)
    , m_skins(skins)
{

}

CSkinAutoAdaptSettingDlg::~CSkinAutoAdaptSettingDlg()
{
}

int CSkinAutoAdaptSettingDlg::FindSkinIndex(const wstring& skin_name)
{
    auto iter = std::find(m_skins.begin(), m_skins.end(), skin_name);
    if (iter == m_skins.end())
        return 0;
    return iter - m_skins.begin();
}

void CSkinAutoAdaptSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DARK_MODE_SKIN_COMBO, m_dark_mode_skin_combo);
    DDX_Control(pDX, IDC_LIGHT_MODE_SKIN_COMBO, m_light_mode_skin_combo);
}


BEGIN_MESSAGE_MAP(CSkinAutoAdaptSettingDlg, CDialog)
END_MESSAGE_MAP()


// CSkinAutoAdaptSettingDlg 消息处理程序


BOOL CSkinAutoAdaptSettingDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    //初始化下拉列表
    for (const auto& skin_path : m_skins)
    {
        wstring skin_name;
        size_t index = skin_path.rfind(L'\\');
        skin_name = skin_path.substr(index + 1);
        m_dark_mode_skin_combo.AddString(skin_name.c_str());
        m_light_mode_skin_combo.AddString(skin_name.c_str());
    }

    int dark_mode_skin = FindSkinIndex(theApp.m_cfg_data.skin_name_dark_mode);
    int light_mode_skin = FindSkinIndex(theApp.m_cfg_data.skin_name_light_mode);
    m_dark_mode_skin_combo.SetCurSel(dark_mode_skin);
    m_light_mode_skin_combo.SetCurSel(light_mode_skin);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CSkinAutoAdaptSettingDlg::OnOK()
{
    m_dark_mode_skin = m_dark_mode_skin_combo.GetCurSel();
    m_light_mode_skin = m_light_mode_skin_combo.GetCurSel();

    CDialog::OnOK();
}
