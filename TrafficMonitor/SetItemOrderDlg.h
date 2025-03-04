#pragma once
#include "BaseDialog.h"

// CSetItemOrderDlg 对话框

class CSetItemOrderDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CSetItemOrderDlg)

public:
    CSetItemOrderDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CSetItemOrderDlg();

    //设置/获取显示顺序
    void SetItemOrder(const std::vector<int>& item_order);
    const std::vector<int>& GetItemOrder() const;

    //设置/获取显示项目
    void SetDisplayItem(const DisplayItemSet& display_item);
    DisplayItemSet GetDisplayItem() const;

    void SetPluginDisplayItem(const StringSet& plugin_item);
    const StringSet& GetPluginDisplayItem() const;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SELECT_ORDER_DIALOG };
#endif

private:
    CTaskbarItemOrderHelper m_item_order;   //显示项目的顺序
    CCheckListBox m_list_ctrl;
    DisplayItemSet m_display_item;        //要显示的内置项目
    StringSet m_plugin_item;            //要显示的插件项目
    std::vector<CommonDisplayItem> m_all_displayed_item;    //在列表中显示的所有项目

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    void ShowItem();
    void EnableCtrl(int list_selected);
    void EnableDlgCtrl(UINT id, bool enable);

    bool GetItemChecked(CommonDisplayItem item);
    void SaveItemChecked(CommonDisplayItem item, bool checked);

    // 通过 CBaseDialog 继承
    virtual CString GetDialogName() const override;

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedMoveUpButton();
    afx_msg void OnBnClickedMoveDownButton();
    afx_msg void OnBnClickedRestoreDefaultButton();
    afx_msg void OnLbnSelchangeList1();
    afx_msg void OnCheckChanged();
    virtual void OnOK();
};
