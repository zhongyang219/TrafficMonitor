#include "stdafx.h"
#include "DrawCommon.h"


CDrawCommon::CDrawCommon()
{
}

CDrawCommon::~CDrawCommon()
{
}

void CDrawCommon::Create(CDC * pDC, CWnd * pMainWnd)
{
	m_pDC = pDC;
	m_pMainWnd = pMainWnd;
	if(pMainWnd!=nullptr)
		m_pfont = m_pMainWnd->GetFont();
}

void CDrawCommon::SetFont(CFont * pfont)
{
	m_pfont = pfont;
}

void CDrawCommon::SetDC(CDC * pDC)
{
	m_pDC = pDC;
}

void CDrawCommon::DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, bool center)
{
	m_pDC->SetTextColor(color);
	m_pDC->SetBkMode(TRANSPARENT);
	m_pDC->SelectObject(m_pfont);
	CSize text_size = m_pDC->GetTextExtent(lpszString);
	if (text_size.cx > rect.Width())		//如果文本宽度超过了矩形区域的宽度，则总是左对齐
		m_pDC->DrawText(lpszString, rect, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	else
		m_pDC->DrawText(lpszString, rect, (center ? DT_CENTER : 0) | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
}


void CDrawCommon::SetDrawRect(CRect rect)
{
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	m_pDC->SelectClipRgn(&rgn);
}

void CDrawCommon::DrawBitmap(CBitmap & bitmap, CPoint start_point, CSize size, StretchMode stretch_mode)
{
	CDC memDC;

	//获取图像实际大小
	BITMAP bm;
	GetObject(bitmap, sizeof(BITMAP), &bm);

	memDC.CreateCompatibleDC(m_pDC);
	memDC.SelectObject(&bitmap);
	// 以下两行避免图片失真
	m_pDC->SetStretchBltMode(HALFTONE);
	m_pDC->SetBrushOrg(0, 0);
	CSize draw_size;
	if (size.cx == 0 || size.cy == 0)		//如果指定的size为0，则使用位图的实际大小绘制
	{
		draw_size = CSize(bm.bmWidth, bm.bmHeight);
	}
	else
	{
		draw_size = size;
		if (stretch_mode == StretchMode::FILL)
		{
			SetDrawRect(CRect(start_point, draw_size));
			float w_h_radio, w_h_radio_draw;		//图像的宽高比、绘制大小的宽高比
			w_h_radio = static_cast<float>(bm.bmWidth) / bm.bmHeight;
			w_h_radio_draw = static_cast<float>(size.cx) / size.cy;
			if (w_h_radio > w_h_radio_draw)		//如果图像的宽高比大于绘制区域的宽高比，则需要裁剪两边的图像
			{
				int image_width;		//按比例缩放后的宽度
				image_width = bm.bmWidth * draw_size.cy / bm.bmHeight;
				start_point.x -= ((image_width - draw_size.cx) / 2);
				draw_size.cx = image_width;
			}
			else
			{
				int image_height;		//按比例缩放后的高度
				image_height = bm.bmHeight * draw_size.cx / bm.bmWidth;
				start_point.y -= ((image_height - draw_size.cy) / 2);
				draw_size.cy = image_height;
			}
		}
		else if (stretch_mode == StretchMode::FIT)
		{
			draw_size = CSize(bm.bmWidth, bm.bmHeight);
			float w_h_radio, w_h_radio_draw;		//图像的宽高比、绘制大小的宽高比
			w_h_radio = static_cast<float>(bm.bmWidth) / bm.bmHeight;
			w_h_radio_draw = static_cast<float>(size.cx) / size.cy;
			if (w_h_radio > w_h_radio_draw)		//如果图像的宽高比大于绘制区域的宽高比
			{
				draw_size.cy = draw_size.cy * size.cx / draw_size.cx;
				draw_size.cx = size.cx;
				start_point.y += ((size.cy - draw_size.cy) / 2);
			}
			else
			{
				draw_size.cx = draw_size.cx * size.cy / draw_size.cy;
				draw_size.cy = size.cy;
				start_point.x += ((size.cx - draw_size.cx) / 2);
			}
		}
	}

	m_pDC->StretchBlt(start_point.x, start_point.y, draw_size.cx, draw_size.cy, &memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	memDC.DeleteDC();
}

void CDrawCommon::DrawBitmap(UINT bitmap_id, CPoint start_point, CSize size, StretchMode stretch_mode)
{
	CDC memDC;
	CBitmap bitmap;
	bitmap.LoadBitmap(bitmap_id);
	DrawBitmap(bitmap, start_point, size, stretch_mode);
}

void CDrawCommon::DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode)
{
	//CDC memDC;
	CBitmap bitmap;
	if (!bitmap.Attach(hbitmap))
		return;
	DrawBitmap(bitmap, start_point, size, stretch_mode);
	bitmap.Detach();
}

void CDrawCommon::FillRect(CRect rect, COLORREF color)
{
	m_pDC->FillSolidRect(rect, color);
}
