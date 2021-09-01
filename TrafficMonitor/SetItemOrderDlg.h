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

    //设置/获取显示项目，使用unsigned int的每个bit表示对应项目是否显示
    void SetDisplayItem(unsigned int display_item);
    unsigned int GetDisplayItem() const;

    void SetPluginDisplayItem(const StringSet& plugin_item);
    const StringSet& GetPluginDisplayItem() const;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECT_ORDER_DIALOG };
#endif

private:
    CTaskbarItemOrderHelper m_item_order;
    CCheckListBox m_list_ctrl;
    unsigned int m_display_item;
    StringSet m_plugin_item;
    int m_item_coumt{};             //除插件项目以外的项目的个数

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    void ShowItem();
    void EnableCtrl(int list_selected);
    void EnableDlgCtrl(UINT id, bool enable);

    // 通过 CBaseDialog 继承
    virtual CString GetDialogName() const override;

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedMoveUpButton();
    afx_msg void OnBnClickedMoveDownButton();
    afx_msg void OnBnClickedRestoreDefaultButton();
    afx_msg void OnLbnSelchangeList1();
    virtual void OnOK();
};
