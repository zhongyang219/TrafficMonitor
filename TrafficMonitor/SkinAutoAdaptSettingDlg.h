#pragma once
#include "afxdialogex.h"


// CSkinAutoAdaptSettingDlg 对话框

class CSkinAutoAdaptSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSkinAutoAdaptSettingDlg)

public:
	CSkinAutoAdaptSettingDlg(const vector<wstring>& skins, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSkinAutoAdaptSettingDlg();

    int GetDarkModeSkin() const { return m_dark_mode_skin; }
    int GetLightModeSkin() const { return m_light_mode_skin; }

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SKIN_AUTO_ADAPT_DLG };
#endif

private:
    int FindSkinIndex(const wstring& skin_name);

private:
    const vector<wstring>& m_skins;
    CComboBox m_dark_mode_skin_combo;
    CComboBox m_light_mode_skin_combo;

    int m_dark_mode_skin{};
    int m_light_mode_skin{};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
};
