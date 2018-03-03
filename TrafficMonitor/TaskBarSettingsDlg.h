#pragma once


// CTaskBarSettingsDlg 对话框

class CTaskBarSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskBarSettingsDlg)

public:
	CTaskBarSettingsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTaskBarSettingsDlg();

	//选项设置数据
	TaskBarSettingData m_data;

	void DrawStaticColor();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASKBAR_SETTINGS_DIALOG };
#endif

protected:
	CStatic m_text_color_static;
	CStatic m_back_color_static;
	CToolTipCtrl m_toolTip;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSetFontButton1();
	afx_msg void OnBnClickedSetColorButton2();
	afx_msg void OnBnClickedSetColorButton3();
	afx_msg void OnEnChangeUploadEdit1();
	afx_msg void OnEnChangeDownloadEdit1();
	afx_msg void OnEnChangeCpuEdit1();
	afx_msg void OnEnChangeMemoryEdit1();
	afx_msg void OnBnClickedSetDefaultButton1();
	afx_msg void OnBnClickedSwitchUpDownCheck1();
	afx_msg void OnBnClickedTaskbarWndOnLeftCheck();
	afx_msg void OnBnClickedSpeedShortModeCheck();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
