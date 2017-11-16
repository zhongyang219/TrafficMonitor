#pragma once


// CMainWndSettingsDlg 对话框

class CMainWndSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainWndSettingsDlg)

public:
	CMainWndSettingsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainWndSettingsDlg();

	//选项设置数据
	CString m_font_name;
	int m_font_size;
	COLORREF m_text_color;
	wstring m_up_string;		//默认为“上传: ”
	wstring m_down_string;		//默认为“下载: ”
	wstring m_cpu_string;		//默认为“CPU: ”
	wstring m_memory_string;	//默认为“内存: ”
	bool m_swap_up_down;		//是否交换上传和下载的位置
	bool m_hide_main_wnd_when_fullscreen;		//有程序全屏运行时隐藏悬浮窗

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
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnStnClickedTextColorStatic();
	afx_msg void OnBnClickedSetColorButton1();
	afx_msg void OnBnClickedSetDefaultButton();
	afx_msg void OnBnClickedSetFontButton();
	afx_msg void OnBnClickedSwitchUpDownCheck();
	afx_msg void OnBnClickedFullscreenHideCheck();
};
