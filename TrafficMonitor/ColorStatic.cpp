// ColorStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "ColorStatic.h"
#include "DrawCommon.h"

// CColorStatic

IMPLEMENT_DYNAMIC(CColorStatic, CStatic)

CColorStatic::CColorStatic()
{

}

CColorStatic::~CColorStatic()
{
}

void CColorStatic::SetFillColor(COLORREF fill_color)
{
	m_colors.resize(1);
	m_colors[0] = fill_color;
	//m_fill_color = fill_color;
	Invalidate();
}

void CColorStatic::SetColorNum(int color_num)
{
	if (color_num <= 0 || color_num > 32)
		color_num = 1;
	m_colors.resize(color_num);
}

void CColorStatic::SetFillColor(int index, COLORREF fill_color)
{
	if (index >= 0 && index < static_cast<int>(m_colors.size()))
		m_colors[index] = fill_color;
}

void CColorStatic::SetLinkCursor(bool link_cursor)
{
	m_link_cursor = link_cursor;
}

void CColorStatic::EnableWindow(bool enable)
{
    CStatic::EnableWindow(enable);
    Invalidate(FALSE);
}


BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CColorStatic 消息处理程序




void CColorStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CStatic::OnPaint()
	CRect rect;
	GetClientRect(rect);
	rect.MoveToXY(0, 0);
	CRect rc_tmp{ rect };

	CDrawCommon draw;
	draw.Create(&dc, this);

    int color_num = static_cast<int>(m_colors.size());

	if (IsWindowEnabled() && color_num > 0)
	{
		if (color_num == 1)
		{
			dc.FillSolidRect(rect, m_colors[0]);
		}
		//颜色数量大于或等于4，并且是4个位数时，上下两行以“Z”字形排列
		else if (color_num >= 4 && color_num % 4 == 0)
		{
			dc.FillSolidRect(rect, RGB(255, 255, 255));
			int group_num = color_num / 4;		//颜色组数（4个为一组）
			for (int i = 0; i < group_num; i++)
			{
				int group_left = rect.Width() * i / group_num;
				int group_right = rect.Width() * (i + 1) / group_num;
				CRect rc_group(group_left, 0, group_right, rect.Height());	//当前组的矩形区域
				CRect rc1(group_left, 0, group_left + rc_group.Width() / 2, rc_group.Height() / 2);
				CRect rc2(group_left + rc_group.Width() / 2, 0, rc_group.right, rc_group.Height() / 2);
				CRect rc3(group_left, rc_group.Height() / 2, group_left + rc_group.Width() / 2, rc_group.bottom);
				CRect rc4(group_left + rc_group.Width() / 2, rc_group.Height() / 2, rc_group.right, rc_group.bottom);
				dc.FillSolidRect(rc1, m_colors[static_cast<size_t>(i) * 4]);
				dc.FillSolidRect(rc2, m_colors[static_cast<size_t>(i) * 4 + 1]);
				dc.FillSolidRect(rc3, m_colors[static_cast<size_t>(i) * 4 + 2]);
				dc.FillSolidRect(rc4, m_colors[static_cast<size_t>(i) * 4 + 3]);
			}
		}
		//其他情况，一行从左到右排列
		else
		{
			dc.FillSolidRect(rect, RGB(255, 255, 255));
			for (int i = 0; i < color_num; i++)
			{
				int left = rect.Width() * i / color_num;
				int right = rect.Width() * (i + 1) / color_num;
				CRect rc_cell(left, 0, right, rect.Height());
				dc.FillSolidRect(rc_cell, m_colors[i]);
			}
		}

		//画边框
		draw.DrawRectOutLine(rect, RGB(160, 160, 160));
	}
	else
	{
        CBrush brush(HS_BDIAGONAL, RGB(160, 160, 160));
        dc.FillRect(rect, &brush);
		draw.DrawRectOutLine(rect, RGB(192, 192, 192));
	}
}


void CColorStatic::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_hover = true;

	CStatic::OnMouseHover(nFlags, point);
}


void CColorStatic::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_hover = false;

	CStatic::OnMouseLeave();
}


BOOL CColorStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_link_cursor && m_hover)
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}


void CColorStatic::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	DWORD dwStyle = GetStyle();
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
	ModifyStyleEx(WS_EX_STATICEDGE, NULL);

	CStatic::PreSubclassWindow();
}


void CColorStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_link_cursor)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		_TrackMouseEvent(&tme);
	}

	CStatic::OnMouseMove(nFlags, point);
}


void CColorStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//如果单击了鼠标左键，则向父窗口发送一个WM_STATIC_CLICKED消息
	CWnd* pParent{ GetParent() };
	if (pParent != nullptr)
		pParent->SendMessage(WM_STATIC_CLICKED, (WPARAM)this);

	CStatic::OnLButtonUp(nFlags, point);
}
