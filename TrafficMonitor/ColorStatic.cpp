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
	m_color_num = 1;
	//m_fill_color = fill_color;
	Invalidate();
}

void CColorStatic::SetColorNum(int color_num)
{
	if (color_num <= 0 || color_num > 10)
		color_num = 1;
	m_colors.resize(color_num);
	m_color_num = color_num;
}

void CColorStatic::SetFillColor(int index, COLORREF fill_color)
{
	if (index >= 0 && index < m_color_num)
		m_colors[index] = fill_color;
}

void CColorStatic::SetLinkCursor(bool link_cursor)
{
	m_link_cursor = link_cursor;
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

	if (IsWindowEnabled())
	{
		switch (m_color_num)
		{
		case 1:
			dc.FillSolidRect(rect, m_colors[0]);
			break;
		case 4:
			dc.FillSolidRect(rect, RGB(255,255,255));
			rc_tmp.right /= 2;
			rc_tmp.bottom /= 2;
			dc.FillSolidRect(rc_tmp, m_colors[0]);
			rc_tmp.MoveToX(rc_tmp.right);
			dc.FillSolidRect(rc_tmp, m_colors[1]);
			rc_tmp.MoveToXY(0, rc_tmp.bottom);
			dc.FillSolidRect(rc_tmp, m_colors[2]);
			rc_tmp.MoveToX(rc_tmp.right);
			dc.FillSolidRect(rc_tmp, m_colors[3]);
			break;
		case 8:
			dc.FillSolidRect(rect, RGB(255, 255, 255));
			rc_tmp.right /= 4;
			rc_tmp.bottom /= 2;
			dc.FillSolidRect(rc_tmp, m_colors[0]);
			rc_tmp.MoveToX(rc_tmp.right);
			dc.FillSolidRect(rc_tmp, m_colors[1]);
			rc_tmp.MoveToX(rc_tmp.right);
			dc.FillSolidRect(rc_tmp, m_colors[4]);
			rc_tmp.MoveToX(rc_tmp.right);
			dc.FillSolidRect(rc_tmp, m_colors[5]);
			rc_tmp.MoveToXY(0, rc_tmp.bottom);
			dc.FillSolidRect(rc_tmp, m_colors[2]);
			rc_tmp.MoveToX(rc_tmp.right);
			dc.FillSolidRect(rc_tmp, m_colors[3]);
			rc_tmp.MoveToX(rc_tmp.right);
			dc.FillSolidRect(rc_tmp, m_colors[6]);
			rc_tmp.MoveToX(rc_tmp.right);
			dc.FillSolidRect(rc_tmp, m_colors[7]);
			break;
		default:
			dc.FillSolidRect(rect, RGB(255, 255, 255));
			rc_tmp.right = rect.Width() / m_color_num;
			for (int i{}; i < m_color_num; i++)
			{
				rc_tmp.MoveToX(i*(rect.Width() / m_color_num));
				dc.FillSolidRect(rc_tmp, m_colors[i]);
			}
		}

		//画边框
		draw.DrawRectOutLine(rect, RGB(160, 160, 160));
	}
	else
	{
		dc.FillSolidRect(rect, RGB(225, 225, 225));
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
