#pragma once
#include "BaseDialog.h"

// CPluginManagerDlg 对话框

class CPluginManagerDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CPluginManagerDlg)

public:
	CPluginManagerDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPluginManagerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLUGIN_MANAGER_DIALOG };
#endif

private:
    CListCtrl m_list_ctrl;
    int m_item_selected{ -1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    void EnableControl();

	DECLARE_MESSAGE_MAP()

    // 通过 CBaseDialog 继承
    virtual CString GetDialogName() const override;
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClickList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedOptinsButton();
    afx_msg void OnBnClickedPluginInfoButton();
    afx_msg void OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
};
