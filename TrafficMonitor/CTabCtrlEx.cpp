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

	InsertItem(m_tab_list.size(), lable_text);

	pWnd->SetParent(this);
	pWnd->MoveWindow(m_tab_rect);

	m_tab_list.push_back(pWnd);
}

void CTabCtrlEx::SetCurTab(int index)
{
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


BEGIN_MESSAGE_MAP(CTabCtrlEx, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, &CTabCtrlEx::OnTcnSelchange)
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
	GetClientRect(m_tab_rect);
	CRect rcTabItem;
	GetItemRect(0, rcTabItem);
	m_tab_rect.top += rcTabItem.Height() + 4;
	m_tab_rect.left += 4;
	m_tab_rect.bottom -= 4;
	m_tab_rect.right -= 4;

	CTabCtrl::PreSubclassWindow();
}
