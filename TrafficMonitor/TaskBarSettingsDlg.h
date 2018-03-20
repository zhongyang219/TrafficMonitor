#pragma once
#include "StaticEx.h"
#include "afxwin.h"

// CTaskBarSettingsDlg �Ի���

class CTaskBarSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskBarSettingsDlg)

public:
	CTaskBarSettingsDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTaskBarSettingsDlg();

	//ѡ����������
	TaskBarSettingData m_data;

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASKBAR_SETTINGS_DIALOG };
#endif

protected:
	CStaticEx m_text_color_static;
	CStaticEx m_back_color_static;
	CToolTipCtrl m_toolTip;
	CComboBox m_unit_combo;
	CButton m_hide_unit_chk;

	void DrawStaticColor();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	afx_msg void OnCbnSelchangeUnitCombo();
	afx_msg void OnBnClickedHideUnitCheck();
};
