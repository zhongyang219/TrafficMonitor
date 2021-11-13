#pragma once
#include "BaseDialog.h"

// CPluginInfoDlg 对话框

class CPluginInfoDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CPluginInfoDlg)

public:
	CPluginInfoDlg(int plugin_index, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPluginInfoDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NETWORK_INFO_DIALOG };
#endif

private:
    int m_cur_index;        //初始显示的插件索引
    CListCtrl m_info_list;
    CMenu m_menu;
    CString m_selected_string;

    //列表中的列
    enum RowIndex
    {
        RI_FILE_NAME,
        RI_FILE_PATH,
        RI_NAME,
        RI_DESCRIPTION,
        RI_ITEM_NUM,
        RI_ITEM_NAMES,
        RI_ITEM_ID,
        RI_AUTHOR,
        RI_COPYRIGHT,
        RI_URL,
        RI_VERSION,
        RI_API_VERSION,
        RI_MAX
    };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    void ShowInfo();
    static CString GetRowName(int row_index);   //获取行的名称

    // 通过 CBaseDialog 继承
    virtual CString GetDialogName() const override;

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();

    afx_msg void OnCopyText();
    afx_msg void OnNMRClickInfoList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedPreviousButton();
    afx_msg void OnBnClickedNextButton();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

};
