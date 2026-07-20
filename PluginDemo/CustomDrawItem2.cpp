#include "pch.h"
#include "CustomDrawItem2.h"
#include "DataManager.h"
#include "PluginDemo.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

static HBITMAP LoadPngResource(HINSTANCE hInstance, int resourceId)
{
    HBITMAP hBitmap = NULL;

    HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resourceId), L"PNG");
    if (!hResource) return NULL;

    HGLOBAL hMemory = LoadResource(hInstance, hResource);
    if (!hMemory) return NULL;

    DWORD size = SizeofResource(hInstance, hResource);
    void* pData = LockResource(hMemory);
    if (!pData) return NULL;

    // 创建 IStream
    IStream* pStream = NULL;
    // 使用 CreateStreamOnHGlobal 将内存块包装成流
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, size);
    if (hGlobal) {
        void* pBuffer = GlobalLock(hGlobal);
        if (pBuffer) {
            memcpy(pBuffer, pData, size);
            GlobalUnlock(hGlobal);
            // 创建流，第二个参数 TRUE 表示流拥有内存，自动释放
            if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK) {
                // 用 GDI+ 的 Bitmap 从流加载
                Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap(pStream);
                if (pBitmap->GetLastStatus() == Gdiplus::Ok) {
                    Gdiplus::Color background(0, 0, 0, 0);
                    pBitmap->GetHBITMAP(background, &hBitmap);
                }
                delete pBitmap;
                pStream->Release();
            }
        }
    }

    // 释放资源
    FreeResource(hMemory);

    return hBitmap;
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
    pDrawer->DrawBitmap(m_png, png_rect.left, png_rect.top, png_rect.Width(), png_rect.Height());
    
    //绘制文本
    CRect text_rect(rect);
    text_rect.left = png_rect.right + CPluginDemo::Instance().DPI(4);
    //pDrawer->DrawRectOutLine(text_rect.left, text_rect.top, text_rect.Width(), text_rect.Height(), text_color);
    pDrawer->DrawWindowText(text_rect.left, text_rect.top, text_rect.Width(), text_rect.Height(), L"test", text_color);
    return true;
}
