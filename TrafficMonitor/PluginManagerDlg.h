#pragma once
#include "BaseDialog.h"
#include "LinkStatic.h"
#include "ListCtrlEx.h"

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
    CListCtrlEx m_list_ctrl;
    int m_item_selected{ -1 };
    CMenu m_menu;
    CLinkStatic m_plugin_download_lnk;
    CLinkStatic m_plugin_dev_guide_lnk;
    CLinkStatic m_open_plugin_dir_lnk;
    CImageList m_plugin_icon_list;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    void EnableControl();
    bool IsSelectedValid();
    bool IsSelectedPluginEnable();     //选中插件是否可用

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
    afx_msg void OnInitMenu(CMenu* pMenu);
    afx_msg void OnPluginDetail();
    afx_msg void OnPluginOptions();
    afx_msg void OnPluginDisable();
protected:
    afx_msg LRESULT OnLinkClicked(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
