#include "stdafx.h"
#include "WIC.h"

CWICFactory CWICFactory::m_instance;

CWICFactory::CWICFactory()
{
#ifndef COMPILE_IN_WIN_XP
    //³õÊ¼»¯m_pWICFactory
    _hrOleInit = ::OleInitialize(NULL);
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
    if (m_pWICFactory == nullptr)
        CoCreateInstance(CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
#endif
}

CWICFactory::~CWICFactory()
{
    if (m_pWICFactory)
    {
        m_pWICFactory->Release();
        m_pWICFactory = NULL;
    }

    if (SUCCEEDED(_hrOleInit))
    {
        OleUninitialize();
    }
}


//////////////////////////////////////////////////////////

typedef DWORD ARGB;
std::map<HICON, HBITMAP> CMenuIcon::m_icon_map;

CMenuIcon::CMenuIcon()
{
}

CMenuIcon::~CMenuIcon()
{
}

HRESULT CMenuIcon::AddIconToMenuItem(HMENU hmenu, int iMenuItem, BOOL fByPosition, HICON hicon)
{
#ifndef COMPILE_IN_WIN_XP
    HBITMAP hbmp{};
    HRESULT hr = GetBitmapByIcon(hicon, hbmp);

    if (SUCCEEDED(hr))
    {
        hr = AddBitmapToMenuItem(hmenu, iMenuItem, fByPosition, hbmp);
    }

    return hr;
#else
    return 0;
#endif // !COMPILE_IN_WIN_XP
}

HRESULT CMenuIcon::AddBitmapToMenuItem(HMENU hmenu, int iItem, BOOL fByPosition, HBITMAP hbmp)
{
    HRESULT hr = E_FAIL;

    MENUITEMINFO mii = { sizeof(mii) };
    mii.fMask = MIIM_BITMAP;
    mii.hbmpItem = hbmp;
    if (SetMenuItemInfo(hmenu, iItem, fByPosition, &mii))
    {
        hr = S_OK;
    }

    return hr;
}

void CMenuIcon::InitBitmapInfo(BITMAPINFO * pbmi, ULONG cbInfo, LONG cx, LONG cy, WORD bpp)
{
    ZeroMemory(pbmi, cbInfo);
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biPlanes = 1;
    pbmi->bmiHeader.biCompression = BI_RGB;

    pbmi->bmiHeader.biWidth = cx;
    pbmi->bmiHeader.biHeight = cy;
    pbmi->bmiHeader.biBitCount = bpp;
}

HRESULT CMenuIcon::Create32BitHBITMAP(HDC hdc, const SIZE * psize, void ** ppvBits, HBITMAP * phBmp)
{
    *phBmp = NULL;

    BITMAPINFO bmi;
    InitBitmapInfo(&bmi, sizeof(bmi), psize->cx, psize->cy, 32);

    HDC hdcUsed = hdc ? hdc : GetDC(NULL);
    if (hdcUsed)
    {
        *phBmp = CreateDIBSection(hdcUsed, &bmi, DIB_RGB_COLORS, ppvBits, NULL, 0);
        if (hdc != hdcUsed)
        {
            ReleaseDC(NULL, hdcUsed);
        }
    }
    return (NULL == *phBmp) ? E_OUTOFMEMORY : S_OK;
}

HRESULT CMenuIcon::GetBitmapByIcon(HICON hicon, HBITMAP& hbmp)
{
    auto iter = m_icon_map.find(hicon);
    if (iter != m_icon_map.end())
    {
        hbmp = iter->second;
        return ERROR_SUCCESS;
    }
    else
    {
        if (CWICFactory::GetWIC() == nullptr)
            return 0;
        hbmp = NULL;

        IWICBitmap* pBitmap;
        HRESULT hr = CWICFactory::GetWIC()->CreateBitmapFromHICON(hicon, &pBitmap);
        if (SUCCEEDED(hr))
        {
            IWICFormatConverter* pConverter;
            hr = CWICFactory::GetWIC()->CreateFormatConverter(&pConverter);
            if (SUCCEEDED(hr))
            {
                hr = pConverter->Initialize(pBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
                if (SUCCEEDED(hr))
                {
                    UINT cx, cy;
                    hr = pConverter->GetSize(&cx, &cy);
                    if (SUCCEEDED(hr))
                    {
                        const SIZE sizIcon = { (int)cx, -(int)cy };
                        BYTE* pbBuffer;
                        hr = Create32BitHBITMAP(NULL, &sizIcon, reinterpret_cast<void**>(&pbBuffer), &hbmp);
                        if (SUCCEEDED(hr))
                        {
                            const UINT cbStride = cx * sizeof(ARGB);
                            const UINT cbBuffer = cy * cbStride;
                            hr = pConverter->CopyPixels(NULL, cbStride, cbBuffer, pbBuffer);
                        }
                    }
                }

                pConverter->Release();
            }

            pBitmap->Release();
        }
        if (SUCCEEDED(hr))
        {
            m_icon_map[hicon] = hbmp;
        }
        if (FAILED(hr))
        {
            DeleteObject(hbmp);
            hbmp = NULL;
        }
        return hr;
    }
}
