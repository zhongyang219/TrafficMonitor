#pragma once
#include "afxcmn.h"
#include "DrawCommon.h"
#include <set>

#define IDC_ITEM_EDITBOX 1991
class CListCtrlEx :
	public CListCtrl
{
DECLARE_DYNAMIC(CListCtrlEx)
public:
	CListCtrlEx();
	~CListCtrlEx();

    void Edit(int row, int col);			//编辑指定单元格

    enum eEditColMethod       //要编辑的列的方式
    {
        EC_NONE,        //无
        EC_ALL,         //全部
        EC_SPECIFIED    //指定的列
    };
    void SetEditColMethod(eEditColMethod method);
    void SetEditableCol(const std::initializer_list<int>& paras);   //设置允许编辑的列

private:
    CEdit m_item_edit;
    int m_edit_row{};
    int m_edit_col{};
    bool m_editing{};

    eEditColMethod m_edit_col_method{ EC_NONE };
    std::set<int> m_edit_cols;

protected:
    void EndEdit();

	DECLARE_MESSAGE_MAP()
    afx_msg void OnEnKillfocusEdit1();
    virtual void PreSubclassWindow();
    afx_msg BOOL OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnBeginScroll(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg LRESULT OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam);
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};

