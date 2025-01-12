#include "stdafx.h"
#include "DrawCommonEx.h"
#include "DrawCommon.h"

CDrawCommonEx::CDrawCommonEx(CDC* pDC)
{
    Create(pDC);
}

CDrawCommonEx::CDrawCommonEx()
{
}


CDrawCommonEx::~CDrawCommonEx()
{
    SAFE_DELETE(m_pGraphics);
}

void CDrawCommonEx::Create(CDC* pDC)
{
    ASSERT(pDC != nullptr);
    m_pDC = pDC;
    SAFE_DELETE(m_pGraphics);
    m_pGraphics = new Gdiplus::Graphics(pDC->GetSafeHdc());
}

void CDrawCommonEx::SetFont(CFont * pFont)
{
    //将字体设置到CDC，绘图时从CDC创建GDI+字体
    m_pDC->SelectObject(pFont);
}

void CDrawCommonEx::DrawImage(Gdiplus::Image* pImage, CPoint start_point, CSize size, StretchMode stretch_mode)
{
    m_pGraphics->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQuality);
    DrawCommonHelper::ImageDrawAreaConvert(CSize(pImage->GetWidth(), pImage->GetHeight()), start_point, size, stretch_mode);
    m_pGraphics->DrawImage(pImage, INT(start_point.x), INT(start_point.y), INT(size.cx), INT(size.cy));
}

void CDrawCommonEx::SetBackColor(COLORREF back_color, BYTE alpha)
{
    m_back_color = CGdiPlusHelper::COLORREFToGdiplusColor(back_color, alpha);
}

void CDrawCommonEx::DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align, bool draw_back_ground, bool multi_line, BYTE alpha)
{
    //矩形区域
    Gdiplus::RectF rect_gdiplus = CGdiPlusHelper::CRectToGdiplusRect(rect);

    //绘制背景
    if (draw_back_ground)
    {
        Gdiplus::SolidBrush brush(m_back_color);
        m_pGraphics->FillRectangle(&brush, rect_gdiplus);
    }
    //设置字体
    Gdiplus::Font font(m_pDC->GetSafeHdc());
    //设置文本颜色
    Gdiplus::SolidBrush brush(CGdiPlusHelper::COLORREFToGdiplusColor(color, alpha));
    //设置对齐方式
    Gdiplus::StringFormat format;
    Gdiplus::StringAlignment alignment = Gdiplus::StringAlignmentNear;
    if (align == Alignment::CENTER)
        alignment = Gdiplus::StringAlignmentCenter;
    else if (align == Alignment::RIGHT)
        alignment = Gdiplus::StringAlignmentFar;
    format.SetAlignment(alignment);    //水平对齐方式
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);    //垂直对齐方式
    UINT flags = Gdiplus::StringFormatFlagsNoFitBlackBox;
    if (!multi_line)
        flags |= Gdiplus::StringFormatFlagsNoWrap;      //不自动换行
    format.SetTrimming(Gdiplus::StringTrimmingNone);    //禁止文本截断
    format.SetFormatFlags(flags);

    //绘制文本
    m_pGraphics->DrawString(lpszString, -1, &font, rect_gdiplus, &format, &brush);
}

void CDrawCommonEx::SetDrawRect(CRect rect)
{
    m_pGraphics->SetClip(CGdiPlusHelper::CRectToGdiplusRect(rect));
}

void CDrawCommonEx::FillRect(CRect rect, COLORREF color, BYTE alpha)
{
    Gdiplus::RectF rect_gdiplus = CGdiPlusHelper::CRectToGdiplusRect(rect);
    Gdiplus::SolidBrush brush(CGdiPlusHelper::COLORREFToGdiplusColor(color, alpha));
    m_pGraphics->FillRectangle(&brush, rect_gdiplus);
}

void CDrawCommonEx::DrawRectOutLine(CRect rect, COLORREF color, int width, bool dot_line, BYTE alpha)
{
}

void CDrawCommonEx::DrawLine(CPoint start_point, int height, COLORREF color, BYTE alpha)
{
}

void CDrawCommonEx::SetTextColor(const COLORREF color, BYTE alpha)
{
    m_text_color = CGdiPlusHelper::COLORREFToGdiplusColor(color, alpha);
}

CDC* CDrawCommonEx::GetDC()
{
    return m_pDC;
}

int CDrawCommonEx::GetTextWidth(LPCTSTR lpszString)
{
    Gdiplus::Font font(m_pDC->GetSafeHdc());
    Gdiplus::RectF textSize;
    m_pGraphics->MeasureString(lpszString, -1, &font, Gdiplus::PointF(0, 0), &textSize);
    return textSize.Width;
}

void CDrawCommonEx::DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode, BYTE alpha)
{
}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
Gdiplus::Color CGdiPlusHelper::COLORREFToGdiplusColor(COLORREF color, BYTE alpha /*= 255*/)
{
    return Gdiplus::Color(alpha, GetRValue(color), GetGValue(color), GetBValue(color));
}

COLORREF CGdiPlusHelper::GdiplusColorToCOLORREF(Gdiplus::Color color)
{
    return RGB(color.GetR(), color.GetG(), color.GetB());
}

CRect CGdiPlusHelper::GdiplusRectToCRect(Gdiplus::RectF rect)
{
    return CRect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
}

Gdiplus::RectF CGdiPlusHelper::CRectToGdiplusRect(CRect rect)
{
    return Gdiplus::RectF(rect.left, rect.top, rect.Width(), rect.Height());
}
