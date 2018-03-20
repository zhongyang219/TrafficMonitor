#pragma once


// CGeneralSettingsDlg dialog

class CGeneralSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGeneralSettingsDlg)

public:
	CGeneralSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGeneralSettingsDlg();

	//ѡ����������
	GeneralSettingData m_data;

	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GENERAL_SETTINGS_DIALOG };
#endif

public:
	bool IsAutoRunModified() const { return m_auto_run_modified; }

protected:
	bool m_auto_run_modified{ false };		//��������˿����Զ����е����ã������Ϊtrue

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckNowButton();
	afx_msg void OnBnClickedCheckUpdateCheck();
	afx_msg void OnBnClickedAutoRunCheck();
};
