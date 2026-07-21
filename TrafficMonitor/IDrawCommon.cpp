#include "stdafx.h"
#include "IDrawCommon.h"

void IDrawCommon::DrawWindowText(int x, int y, int w, int h, const wchar_t* lpszString, unsigned long color, Alignment align, bool multi_line, unsigned char alpha)
{
    CRect rect(POINT(x, y), SIZE(w, h));
    DrawWindowText(rect, lpszString, color, align, false, multi_line, alpha);
}

void IDrawCommon::SetDrawRect(int x, int y, int w, int h)
{
    CRect rect(POINT(x, y), SIZE(w, h));
    SetDrawRect(rect);
}

void IDrawCommon::FillRect(int x, int y, int w, int h, unsigned long color, unsigned char alpha)
{
    CRect rect(POINT(x, y), SIZE(w, h));
    FillRect(rect, color, alpha);
}

void IDrawCommon::DrawRectOutLine(int x, int y, int w, int h, unsigned long color, int width, bool dot_line, unsigned char alpha, int radius)
{
    CRect rect(POINT(x, y), SIZE(w, h));
    DrawRectOutLine(rect, color, width, dot_line, alpha, radius);
}

void IDrawCommon::DrawBitmap(void* hbitmap, int x, int y, int w, int h, StretchMode stretch_mode, unsigned char alpha)
{
    DrawBitmap((HBITMAP)hbitmap, CPoint(x, y), CSize(w, h), stretch_mode, alpha);
}

void IDrawCommon::DrawIcon(void* hIcon, int x, int y, int w, int h)
{
    DrawIcon((HICON)hIcon, CPoint(x, y), CSize(w, h));
}

void IDrawCommon::DrawLine(CPoint start_point, int height, COLORREF color, BYTE alpha)
{
    if (height > 0)
    {
        CPoint end_point = start_point;
        end_point.y -= height;
        DrawLine(start_point, end_point, color, alpha);
    }
}

void IDrawCommon::DrawLine(int x1, int y1, int x2, int y2, unsigned long color, unsigned char alpha)
{
    CPoint start_point(x1, y1);
    CPoint end_point(x2, y2);
    DrawLine(start_point, end_point, color, alpha);
}
