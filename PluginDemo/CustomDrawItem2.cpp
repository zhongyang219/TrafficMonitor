#include "pch.h"
#include "CustomDrawItem2.h"
#include "DataManager.h"
#include "PluginDemo.h"

const wchar_t* CCustomDrawItem2::GetItemName() const
{
    return CDataManager::Instance().StringRes(IDS_CUSTOM_DRAW_ITEM2);
}

const wchar_t* CCustomDrawItem2::GetItemId() const
{
    return L"d0L5IidC";
}

const wchar_t* CCustomDrawItem2::GetItemLableText() const
{
    return L"";
}

const wchar_t* CCustomDrawItem2::GetItemValueText() const
{
    return L"";
}

const wchar_t* CCustomDrawItem2::GetItemValueSampleText() const
{
    return L"";
}

bool CCustomDrawItem2::IsCustomDraw() const
{
    return true;
}

int CCustomDrawItem2::GetItemWidth() const
{
    return 60;
}

bool CCustomDrawItem2::DrawItemEx(IPluginDrawer* pDrawer, int x, int y, int w, int h, bool dark_mode)
{
    CRect rect(CPoint(x, y), CSize(w, h));
    COLORREF text_color = dark_mode ? RGB(255, 198, 105) : RGB(160, 99, 0);
    pDrawer->DrawRectOutLine(x, y, w, h, text_color);
    //绘制图标
    CRect icon_rect(rect);
    int icon_size = CPluginDemo::Instance().DPI(16);
    icon_rect.left += CPluginDemo::Instance().DPI(2);
    icon_rect.top = y + (h - icon_size) / 2;
    icon_rect.bottom = icon_rect.top + icon_size;
    icon_rect.right = icon_rect.left + icon_size;
    HICON hIcon = CPluginDemo::Instance().GetIcon(IDI_ICON1);
    pDrawer->DrawIcon(hIcon, icon_rect.left, icon_rect.top, icon_rect.Width(), icon_rect.Height());
    //pDrawer->DrawRectOutLine(icon_rect.left, icon_rect.top, icon_rect.Width(), icon_rect.Height(), text_color);
    //绘制文本
    CRect text_rect(rect);
    text_rect.left = icon_rect.right + CPluginDemo::Instance().DPI(4);
    //pDrawer->DrawRectOutLine(text_rect.left, text_rect.top, text_rect.Width(), text_rect.Height(), text_color);
    pDrawer->DrawWindowText(text_rect.left, text_rect.top, text_rect.Width(), text_rect.Height(), L"test", text_color);
    return true;
}
