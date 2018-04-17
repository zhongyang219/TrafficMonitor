#include "stdafx.h"
#include "StaticEx.h"


CStaticEx::CStaticEx()
{
}


CStaticEx::~CStaticEx()
{
}

void CStaticEx::SetWindowTextEx(LPCTSTR lpszString, Alignment align)
{
	m_text = lpszString;
	m_align = align;
	m_color_text = true;
	Invalidate();
}

void CStaticEx::SetTextColor(COLORREF textColor)
{
	m_text_color = textColor;
	Invalidate();
}

void CStaticEx::SetBackColor(COLORREF back_color)
{
	m_back_color = back_color;
	m_draw_background_color = true;
}

CString CStaticEx::GetString() const
{
	return m_text;
}


LRESULT CStaticEx::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (message == WM_SETTEXT)
	{
		CRect rect;
		CDC* pDC = GetDC();
		GetClientRect(rect);
		DrawThemeParentBackground(m_hWnd, pDC->GetSafeHdc(), &rect);
		ReleaseDC(pDC);
	}
	return CStatic::DefWindowProc(message, wParam, lParam);
}
BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



void CStaticEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CStatic::OnPaint()
	dc.SetTextColor(m_text_color);
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(this->GetFont());
	CRect rect;
	this->GetClientRect(&rect);
	if (m_draw_background_color)
		dc.FillSolidRect(rect, m_back_color);
	else
		DrawThemeParentBackground(m_hWnd, dc.GetSafeHdc(), &rect);	//重绘控件区域以解决文字重叠的问题
	CSize text_size = dc.GetTextExtent(m_text);
	UINT format{ DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX };		//CDC::DrawText()函数的文本格式
	if (text_size.cx > rect.Width())		//如果文本宽度超过了矩形区域的宽度，设置了居中时左对齐
	{
		if (m_align == Alignment::RIGHT)
			format |= DT_RIGHT;
	}
	else
	{
		switch (m_align)
		{
		case Alignment::RIGHT: format |= DT_RIGHT; break;
		case Alignment::CENTER: format |= DT_CENTER; break;
		}
	}
	dc.DrawText(m_text, rect, format);
}


void CStaticEx::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	GetWindowText(m_text);

	CStatic::PreSubclassWindow();
}
