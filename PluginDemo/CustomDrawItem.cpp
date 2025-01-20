#include "pch.h"
#include "CustomDrawItem.h"
#include "DataManager.h"

const wchar_t* CCustomDrawItem::GetItemName() const
{
    return CDataManager::Instance().StringRes(IDS_CUSTOM_DRAW_ITEM);
}

const wchar_t* CCustomDrawItem::GetItemId() const
{
    return L"4Tc21hGS";
}

const wchar_t* CCustomDrawItem::GetItemLableText() const
{
    return L"";
}

const wchar_t* CCustomDrawItem::GetItemValueText() const
{
    return L"";
}

const wchar_t* CCustomDrawItem::GetItemValueSampleText() const
{
    return L"";
}

bool CCustomDrawItem::IsCustomDraw() const
{
    return true;
}

int CCustomDrawItem::GetItemWidth() const
{
    return 50;
}

static void DrawLine(CDC* pDC, CPoint point1, CPoint point2, COLORREF color)
{
    CPen aPen, * pOldPen;
    aPen.CreatePen(PS_SOLID, 1, color);
    pOldPen = pDC->SelectObject(&aPen);
    pDC->MoveTo(point1);
    pDC->LineTo(point2);
    pDC->SelectObject(pOldPen);
}

void CCustomDrawItem::DrawItem(void* hDC, int x, int y, int w, int h, bool dark_mode)
{
    //绘图句柄
    CDC* pDC = CDC::FromHandle((HDC)hDC);
    //矩形区域
    CRect rect(CPoint(x, y), CSize(w, h));
    //设置颜色
    COLORREF color1{ dark_mode ? RGB(255, 143, 107) : RGB(227, 81, 16) };
    COLORREF color2{ dark_mode ? RGB(183, 241, 96) : RGB(83, 131, 11) };
    COLORREF color3{ dark_mode ? RGB(158, 218, 251) : RGB(6, 111, 168) };
    //显示时、分、秒的矩形区域
    CRect rect1{ rect }, rect2{ rect }, rect3{ rect };
    rect1.bottom = rect.top + rect.Height() / 3;
    rect2 = rect1;
    rect2.MoveToY(rect1.bottom);
    rect3.top = rect2.bottom;
    rect1.DeflateRect(1, 1);
    rect2.DeflateRect(1, 1);
    rect3.DeflateRect(1, 1);
    //根据当前时间计算矩形的宽度
    SYSTEMTIME& time{ CDataManager::Instance().m_system_time };
    int hour_width{ static_cast<int>((time.wHour + time.wMinute / 60.0) * w / 24) };
    int min_width{ static_cast<int>((time.wMinute + time.wSecond / 60.0) * w / 60) };
    int sec_width{ time.wSecond * w / 60 };
    rect1.right = rect1.left + hour_width;
    rect2.right = rect2.left + min_width;
    rect3.right = rect3.left + sec_width;
    //填充矩形
    pDC->FillSolidRect(rect1, color1);
    pDC->FillSolidRect(rect2, color2);
    pDC->FillSolidRect(rect3, color3);
    //绘制刻度
    COLORREF color_scale{ dark_mode ? RGB(225, 225, 225) : RGB(45, 45, 45) };
    for (int i{}; i < 24; i++)
    {
        int x_pos{ i * w / 24 + rect1.left };
        DrawLine(pDC, CPoint(x_pos, rect1.top), CPoint(x_pos, i % 6 == 0 ? rect1.bottom : rect1.top + rect1.Height() / 2), color_scale);
    }
}
