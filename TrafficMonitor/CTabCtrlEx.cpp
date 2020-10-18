// CTabCtrlEx.cpp: 实现文件
//

#include "stdafx.h"
#include "CTabCtrlEx.h"


// CTabCtrlEx

IMPLEMENT_DYNAMIC(CTabCtrlEx, CTabCtrl)

CTabCtrlEx::CTabCtrlEx()
{

}

CTabCtrlEx::~CTabCtrlEx()
{
}

void CTabCtrlEx::AddWindow(CWnd* pWnd, LPCTSTR lable_text)
{
	if (pWnd == nullptr || pWnd->GetSafeHwnd() == NULL)
		return;

    InsertItem(m_tab_list.size(), lable_text, m_tab_list.size());

	pWnd->SetParent(this);
	pWnd->MoveWindow(m_tab_rect);

	m_tab_list.push_back(pWnd);
}

void CTabCtrlEx::SetCurTab(int index)
{
    if (index < 0 || index >= static_cast<int>(m_tab_list.size()))
        index = 0;
	SetCurSel(index);

	int tab_size = m_tab_list.size();
	for (int i = 0; i < tab_size; i++)
	{
		if (i == index)
		{
			m_tab_list[i]->ShowWindow(SW_SHOW);
			m_tab_list[i]->SetFocus();
		}
		else
		{
			m_tab_list[i]->ShowWindow(SW_HIDE);
		}
	}
}

CWnd* CTabCtrlEx::GetCurrentTab()
{
    int cur_tab_index = GetCurSel();
    if (cur_tab_index >= 0 && cur_tab_index < m_tab_list.size())
    {
        return m_tab_list[cur_tab_index];
    }
    return nullptr;
}

void CTabCtrlEx::AdjustTabWindowSize()
{
    CalSubWindowSize();
    for (size_t i{}; i < m_tab_list.size(); i++)
    {
        m_tab_list[i]->MoveWindow(m_tab_rect);
    }
}

void CTabCtrlEx::CalSubWindowSize()
{
    GetClientRect(m_tab_rect);
    CRect rc_temp = m_tab_rect;
    AdjustRect(FALSE, rc_temp);
    int margin = rc_temp.left - m_tab_rect.left;
    CRect rcTabItem;
    GetItemRect(0, rcTabItem);
    m_tab_rect.top += rcTabItem.Height() + margin;
    m_tab_rect.left += margin;
    m_tab_rect.bottom -= margin;
    m_tab_rect.right -= margin;
}


BEGIN_MESSAGE_MAP(CTabCtrlEx, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, &CTabCtrlEx::OnTcnSelchange)
    ON_WM_SIZE()
END_MESSAGE_MAP()



// CTabCtrlEx 消息处理程序




void CTabCtrlEx::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int tab_selected = GetCurSel();
	SetCurTab(tab_selected);

	*pResult = 0;
}


void CTabCtrlEx::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	//计算子窗口的位置
    CalSubWindowSize();

	CTabCtrl::PreSubclassWindow();
}


void CTabCtrlEx::OnSize(UINT nType, int cx, int cy)
{
    CTabCtrl::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    AdjustTabWindowSize();
}
