#pragma once
#include "MainWndSettingsDlg.h"
#include "TaskBarSettingsDlg.h"
#include "GeneralSettingsDlg.h"
#include "afxcmn.h"
#include "CTabCtrlEx.h"

// COptionsDlg 对话框

class COptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(int tab = 0, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COptionsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTIONS_DIALOG };
#endif

	CMainWndSettingsDlg m_tab1_dlg{ this };
	CTaskBarSettingsDlg m_tab2_dlg{ this };
	CGeneralSettingsDlg m_tab3_dlg{ this };

protected:
	CTabCtrlEx m_tab;
	int m_tab_selected;
    std::vector<CTabDlg*> m_tab_vect;
    std::vector<int> m_tab_height;
    CSize m_min_size{};

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
