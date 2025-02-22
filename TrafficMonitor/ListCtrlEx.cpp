#include "stdafx.h"
#include "ListCtrlEx.h"

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
{
}


CListCtrlEx::~CListCtrlEx()
{
}

void CListCtrlEx::Edit(int row, int col)
{
    EnsureVisible(row, FALSE);				//编辑一行时确保该行可见
    m_editing = true;

    m_edit_row = row;
    m_edit_col = col;
    CRect item_rect;
    GetSubItemRect(row, col, LVIR_LABEL, item_rect);	//取得子项的矩形

    CString text = GetItemText(row, col);		//取得子项的内容

    m_item_edit.SetWindowText(text);		//将子项的内容显示到编辑框中
    m_item_edit.ShowWindow(SW_SHOW);		//显示编辑框
    m_item_edit.MoveWindow(item_rect);		//将编辑框移动到子项上面，覆盖在子项上
    m_item_edit.SetFocus();					//使编辑框取得焦点
    m_item_edit.SetSel(0, -1);
}

void CListCtrlEx::SetEditColMethod(eEditColMethod method)
{
    m_edit_col_method = method;
}

void CListCtrlEx::SetEditableCol(const std::initializer_list<int>& paras)
{
    m_edit_cols = paras;
}

void CListCtrlEx::EndEdit()
{
    if (m_editing)
    {
        if (m_edit_row >= 0 && m_edit_row < GetItemCount())
        {
            CString str;
            m_item_edit.GetWindowText(str);	//取得编辑框的内容
            SetItemText(m_edit_row, m_edit_col, str);	//将该内容更新到CListCtrl中
        }
        m_item_edit.ShowWindow(SW_HIDE);//隐藏编辑框
        m_editing = false;
    }
}

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
    ON_EN_KILLFOCUS(IDC_ITEM_EDITBOX, &CListCtrlEx::OnEnKillfocusEdit1)
    ON_NOTIFY_REFLECT_EX(NM_DBLCLK, &CListCtrlEx::OnNMDblclk)
    ON_NOTIFY_REFLECT(LVN_BEGINSCROLL, &CListCtrlEx::OnLvnBeginScroll)
    ON_MESSAGE(WM_TABLET_QUERYSYSTEMGESTURESTATUS, &CListCtrlEx::OnTabletQuerysystemgesturestatus)
END_MESSAGE_MAP()


void CListCtrlEx::OnEnKillfocusEdit1()
{
    //当文本编辑控件控件失去焦点时响应
    EndEdit();
}



void CListCtrlEx::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_item_edit.Create(WS_BORDER | ES_AUTOHSCROLL, CRect(), this, IDC_ITEM_EDITBOX);
    m_item_edit.SetFont(GetFont());

    CListCtrl::PreSubclassWindow();
}


BOOL CListCtrlEx::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码

    //如果双击的列是要编辑的列
    if (m_edit_col_method == EC_ALL
        || (m_edit_col_method == EC_SPECIFIED && m_edit_cols.find(pNMItemActivate->iSubItem) != m_edit_cols.end()))
    {
        Edit(pNMItemActivate->iItem, pNMItemActivate->iSubItem);
        *pResult = 0;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


void CListCtrlEx::OnLvnBeginScroll(NMHDR *pNMHDR, LRESULT *pResult)
{
    // 此功能要求 Internet Explorer 5.5 或更高版本。
    // 符号 _WIN32_IE 必须是 >= 0x0560。
    LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    EndEdit();

    *pResult = 0;
}


afx_msg LRESULT CListCtrlEx::OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam)
{
    return 0;
}


BOOL CListCtrlEx::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        //编辑状态下按回车键结束编辑而不是关闭对话框
        if (m_editing)
        {
            EndEdit();
            return TRUE;
        }
    }

    return CListCtrl::PreTranslateMessage(pMsg);
}
