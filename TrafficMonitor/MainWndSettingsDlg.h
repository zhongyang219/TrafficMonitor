#pragma once


// CMainWndSettingsDlg 对话框

class CMainWndSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainWndSettingsDlg)

public:
	CMainWndSettingsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainWndSettingsDlg();

	//选项设置数据
	MainWndSettingData m_data;

	void DrawStaticColor();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_WND_SETTINGS_DIALOG };
#endif

protected:
	CStatic m_color_static;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeUploadEdit();
	afx_msg void OnEnChangeDownloadEdit();
	afx_msg void OnEnChangeCpuEdit();
	afx_msg void OnEnChangeMemoryEdit();
	afx_msg void OnBnClickedSetColorButton1();
	afx_msg void OnBnClickedSetDefaultButton();
	afx_msg void OnBnClickedSetFontButton();
	afx_msg void OnBnClickedSwitchUpDownCheck();
	afx_msg void OnBnClickedFullscreenHideCheck();
};
