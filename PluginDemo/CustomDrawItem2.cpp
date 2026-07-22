#include "pch.h"
#include "CustomDrawItem2.h"
#include "DataManager.h"
#include "PluginDemo.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

Gdiplus::Image* LoadPngResource(HINSTANCE hInstance, int resourceId)
{
    Gdiplus::Image* pImage = nullptr;
    if (HRSRC hRes = ::FindResource(hInstance, MAKEINTRESOURCE(resourceId), _T("PNG")))
    {
        DWORD imageSize = ::SizeofResource(hInstance, hRes);
        if (HGLOBAL hResData = ::LoadResource(hInstance, hRes))
        {
            LPVOID pResourceData = ::LockResource(hResData);
            // 在缓冲区上创建内存流
            if (IStream* pStream = SHCreateMemStream(static_cast<const BYTE*>(pResourceData), imageSize))
            {
                pImage = Gdiplus::Image::FromStream(pStream);
                pStream->Release();
            }
            ::FreeResource(hResData);
        }
    }
    return pImage;
}

CCustomDrawItem2::CCustomDrawItem2()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HMODULE hModule = reinterpret_cast<HMODULE>(&__ImageBase);
    m_png = LoadPngResource(hModule, IDB_PNG1);

}

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
    return 80;
}

void CCustomDrawItem2::DrawItem(void* hDC, int x, int y, int w, int h, bool dark_mode)
{
    CRect rect(CPoint(x, y), CSize(w, h));
    //绘制PNG图像
    CRect png_rect(rect);
    int icon_size = CPluginDemo::Instance().DPI(16);
    png_rect.top = y + (h - icon_size) / 2;
    png_rect.bottom = png_rect.top + icon_size;
    png_rect.left += CPluginDemo::Instance().DPI(22);
    png_rect.right = png_rect.left + icon_size;

    Gdiplus::Graphics graphics((HDC)hDC);
    graphics.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQuality);
    graphics.DrawImage(m_png, INT(png_rect.left), INT(png_rect.top), INT(png_rect.Width()), INT(png_rect.Height()));
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
    
    //绘制PNG格式图标
    CRect png_rect(icon_rect);
    png_rect.MoveToX(icon_rect.right + CPluginDemo::Instance().DPI(4));
    //pDrawer->DrawRectOutLine(png_rect.left, png_rect.top, png_rect.Width(), png_rect.Height(), text_color);
    //pDrawer->DrawBitmap(m_png, png_rect.left, png_rect.top, png_rect.Width(), png_rect.Height());
    //在DrawItem函数中直接使用GDI+绘制透明PNG图像

    //绘制文本
    CRect text_rect(rect);
    text_rect.left = png_rect.right + CPluginDemo::Instance().DPI(4);
    int text_height = CPluginDemo::Instance().DPI(16);
    text_rect.top = rect.top + (rect.Height() - text_height) / 2;
    text_rect.bottom = text_rect.top + text_height;
    pDrawer->DrawRectOutLine(text_rect.left, text_rect.top, text_rect.Width(), text_rect.Height(), text_color, 1, false, 255, CPluginDemo::Instance().DPI(3));
    pDrawer->DrawWindowText(text_rect.left, text_rect.top, text_rect.Width(), text_rect.Height(), L"test", text_color);
    return true;
}
