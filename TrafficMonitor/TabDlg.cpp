#include "stdafx.h"
#include "TabDlg.h"
#include "TrafficMonitor.h"

IMPLEMENT_DYNAMIC(CTabDlg, CDialogEx)

CTabDlg::CTabDlg(UINT nIDTemplate, CWnd * pParent) : CDialogEx(nIDTemplate, pParent)
{
	m_pParent = pParent;
}


CTabDlg::~CTabDlg()
{
}


BOOL CTabDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
    //由于这是tab标签中的子对话框，因此用户按回车或ESC后不应该响应当前对话框的IDOK或IDCANCEL，
    //而应该响应tab标签的父窗口的IDOK或IDCANCEL命令
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		if (m_pParent != nullptr)
			m_pParent->SendMessage(WM_COMMAND, IDCANCEL);
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (m_pParent != nullptr)
			m_pParent->SendMessage(WM_COMMAND, IDOK);
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CTabDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//将窗口背景设置成白色
	SetBackgroundColor(RGB(255, 255, 255));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

CWnd* CTabDlg::GetParentWindow()
{
    CWnd* pParent = GetParent();
    if (pParent != nullptr)
    {
        return pParent->GetParent();
    }
    return nullptr;
}

void CTabDlg::SetScrollbarInfo(int nPage, int nMax)
{
	//初始化滚动条
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	scrollinfo.nPage = nPage;    //设置滑块大小
	scrollinfo.nMin = 0;
	scrollinfo.nMax = nMax;     //设置滚动条的最大位置
	if (scrollinfo.nMax < 0)
		scrollinfo.nMax = 0;
	scrollinfo.nPos = scrollinfo.nMin;
	SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
}

void CTabDlg::ResetScroll()
{
    SCROLLINFO scrollinfo;
    GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    int step = scrollinfo.nPos - scrollinfo.nMin;
    scrollinfo.nPos = scrollinfo.nMin;
    SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    ScrollWindow(0, step);
}

void CTabDlg::ScrollWindowSimple(int step)
{
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	scrollinfo.nPos -= step;
	if (scrollinfo.nPos < scrollinfo.nMin)
	{
		step = scrollinfo.nPos + step - scrollinfo.nMin;		//如果向上滚动一个距离后小于滚动条的最小位置了，则修正step的值，使窗口滚动到最上方
		scrollinfo.nPos = scrollinfo.nMin;
	}
	if (scrollinfo.nPos + scrollinfo.nPage > scrollinfo.nMax)  //此处一定要注意加上滑块的长度，再作判断
	{
		step -= (scrollinfo.nMax - (scrollinfo.nPos + scrollinfo.nPage));		//如果向上滚动一个距离后大于滚动条的最大位置了，则修正step的值，使窗口滚动到最下方
		if (step > 0)
			step = 0;
		scrollinfo.nPos = scrollinfo.nMax;
	}
	SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	ScrollWindow(0, step);

}

BEGIN_MESSAGE_MAP(CTabDlg, CDialogEx)
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


void CTabDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//参考资料：https://www.cnblogs.com/ranjiewen/p/6013922.html

	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	int unit = 1;
	int step = theApp.DPI(8);
	switch (nSBCode)
	{
	case SB_LINEUP:          //Scroll one line up
		ScrollWindowSimple(unit * step);
		break;
	case SB_LINEDOWN:           //Scroll one line down
		ScrollWindowSimple(-unit * step);
		break;
	case SB_PAGEUP:            //Scroll one page up.
		ScrollWindowSimple(unit * step * 5);
		break;
	case SB_PAGEDOWN:        //Scroll one page down        
		ScrollWindowSimple(-unit * step * 5);
		break;
	case SB_ENDSCROLL:      //End scroll     
		break;
	case SB_THUMBPOSITION:  //Scroll to the absolute position. The current position is provided in nPos
		break;
	case SB_THUMBTRACK:                  //Drag scroll box to specified position. The current position is provided in nPos
		ScrollWindow(0, (scrollinfo.nPos - nPos)*unit);
		scrollinfo.nPos = nPos;
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		break;
	}
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CTabDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int step = theApp.DPI(16);
	if (zDelta > 0)
	{
		ScrollWindowSimple(step);
	}
	if (zDelta < 0)
	{
		ScrollWindowSimple(-step);
	}

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CTabDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    CDialogEx::OnOK();
}
