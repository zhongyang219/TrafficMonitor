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

void CDrawCommon::DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align, bool draw_back_ground, bool multi_line)
{
	m_pDC->SetTextColor(color);
	if(!draw_back_ground)
		m_pDC->SetBkMode(TRANSPARENT);
	m_pDC->SelectObject(m_pfont);
	CSize text_size = m_pDC->GetTextExtent(lpszString);

	UINT format;		//CDC::DrawText()函数的文本格式
	if (multi_line)
		format = DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX;
	else
		format = DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;

	if (text_size.cx > rect.Width())		//如果文本宽度超过了矩形区域的宽度，设置了居中时左对齐
	{
		if (align == Alignment::RIGHT)
			format |= DT_RIGHT;
	}
	else
	{
		switch (align)
		{
		case Alignment::RIGHT: format |= DT_RIGHT; break;
		case Alignment::CENTER: format |= DT_CENTER; break;
		}
	}
	if(draw_back_ground)
		m_pDC->FillSolidRect(rect, m_back_color);
	m_pDC->DrawText(lpszString, rect, format);
}


void CDrawCommon::SetDrawRect(CRect rect)
{
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	m_pDC->SelectClipRgn(&rgn);
}

void CDrawCommon::SetDrawRect(CDC * pDC, CRect rect)
{
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	pDC->SelectClipRgn(&rgn);
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
	CBitmap bitmap;
	bitmap.LoadBitmap(bitmap_id);
	DrawBitmap(bitmap, start_point, size, stretch_mode);
}

void CDrawCommon::DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode)
{
	CBitmap bitmap;
	if (!bitmap.Attach(hbitmap))
		return;
	DrawBitmap(bitmap, start_point, size, stretch_mode);
	bitmap.Detach();
}

void CDrawCommon::BitmapStretch(CImage *pImage, CImage *ResultImage, CSize size)
{
	if (pImage->IsDIBSection())
	{
		// 取得 pImage 的 DC
		CDC* pImageDC1 = CDC::FromHandle(pImage->GetDC()); // Image 因為有自己的 DC, 所以必須使用 FromHandle 取得對應的 DC

		CBitmap *bitmap1 = pImageDC1->GetCurrentBitmap();
		BITMAP bmpInfo;
		bitmap1->GetBitmap(&bmpInfo);

		// 建立新的 CImage
		ResultImage->Create(size.cx, size.cy, bmpInfo.bmBitsPixel);
		CDC* ResultImageDC = CDC::FromHandle(ResultImage->GetDC());

		// 當 Destination 比較小的時候, 會根據 Destination DC 上的 Stretch Blt mode 決定是否要保留被刪除點的資訊
		ResultImageDC->SetStretchBltMode(HALFTONE); // 使用最高品質的方式
		::SetBrushOrgEx(ResultImageDC->m_hDC, 0, 0, NULL); // 調整 Brush 的起點

		// 把 pImage 畫到 ResultImage 上面
		StretchBlt(*ResultImageDC, 0, 0, size.cx, size.cy, *pImageDC1, 0, 0, pImage->GetWidth(), pImage->GetHeight(), SRCCOPY);
		// pImage->Draw(*ResultImageDC,0,0,StretchWidth,StretchHeight,0,0,pImage->GetWidth(),pImage->GetHeight());

		pImage->ReleaseDC();
		ResultImage->ReleaseDC();
	}
}

void CDrawCommon::FillRect(CRect rect, COLORREF color)
{
	m_pDC->FillSolidRect(rect, color);
}

void CDrawCommon::FillRectWithBackColor(CRect rect)
{
	m_pDC->FillSolidRect(rect, m_back_color);
}

void CDrawCommon::DrawRectOutLine(CRect rect, COLORREF color, int width, bool dot_line)
{
	CPen aPen, *pOldPen;
	aPen.CreatePen((dot_line ? PS_DOT : PS_SOLID), width, color);
	pOldPen = m_pDC->SelectObject(&aPen);
	CBrush* pOldBrush{ dynamic_cast<CBrush*>(m_pDC->SelectStockObject(NULL_BRUSH)) };

	rect.DeflateRect(width / 2, width / 2);
	m_pDC->Rectangle(rect);
	m_pDC->SelectObject(pOldPen);
	m_pDC->SelectObject(pOldBrush);       // Restore the old brush
}

void CDrawCommon::GetRegionFromImage(CRgn& rgn, CBitmap &cBitmap, int threshold)
{
	CDC memDC;

	memDC.CreateCompatibleDC(NULL);
	CBitmap *pOldMemBmp = NULL;
	pOldMemBmp = memDC.SelectObject(&cBitmap);

	//创建总的窗体区域，初始region为0
	rgn.CreateRectRgn(0, 0, 0, 0);

	BITMAP bit;
	cBitmap.GetBitmap(&bit);//取得位图参数，这里要用到位图的长和宽
	int y;
	for (y = 0; y<bit.bmHeight; y++)
	{
		CRgn rgnTemp; //保存临时region
		int iX = 0;
		do
		{
			//跳过透明色找到下一个非透明色的点.
			while (iX < bit.bmWidth && GetColorBritness(memDC.GetPixel(iX, y)) <= threshold)
				iX++;
			int iLeftX = iX; //记住这个起始点

							 //寻找下个透明色的点
			while (iX < bit.bmWidth && GetColorBritness(memDC.GetPixel(iX, y)) > threshold)
				++iX;

			//创建一个包含起点与重点间高为1像素的临时“region”
			rgnTemp.CreateRectRgn(iLeftX, y, iX, y + 1);
			rgn.CombineRgn(&rgn, &rgnTemp, RGN_OR);

			//删除临时"region",否则下次创建时和出错
			rgnTemp.DeleteObject();
		} while (iX < bit.bmWidth);
	}
}

int CDrawCommon::GetColorBritness(COLORREF color)
{
	return (GetRValue(color) + GetGValue(color) + GetBValue(color)) / 3;
}
