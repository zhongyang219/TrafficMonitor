#pragma once


// CGeneralSettingsDlg dialog

class CGeneralSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGeneralSettingsDlg)

public:
	CGeneralSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGeneralSettingsDlg();

	//选项设置数据
	GeneralSettingData m_data;

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GENERAL_SETTINGS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckNowButton();
	afx_msg void OnBnClickedCheckUpdateCheck();
};
