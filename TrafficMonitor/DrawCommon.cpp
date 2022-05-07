#include "stdafx.h"
#include "DrawCommon.h"
#include "TrafficMonitor.h"

CDrawCommon::CDrawCommon()
{
}

CDrawCommon::~CDrawCommon()
{
}

void CDrawCommon::Create(CDC* pDC, CWnd* pMainWnd)
{
    m_pDC = pDC;
    m_pMainWnd = pMainWnd;
    if (pMainWnd != nullptr)
        m_pfont = m_pMainWnd->GetFont();
}

void CDrawCommon::SetFont(CFont* pfont)
{
    m_pfont = pfont;
    m_pDC->SelectObject(m_pfont);
}

void CDrawCommon::SetDC(CDC* pDC)
{
    m_pDC = pDC;
}

void CDrawCommon::DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align, bool draw_back_ground, bool multi_line, BYTE alpha)
{
    m_pDC->SetTextColor(color);
    if (!draw_back_ground)
        m_pDC->SetBkMode(TRANSPARENT);
    m_pDC->SelectObject(m_pfont);
    CSize text_size = m_pDC->GetTextExtent(lpszString);

    auto format = DrawCommonHelper::ProccessTextFormat(rect, text_size, align, multi_line);

    if (draw_back_ground)
        m_pDC->FillSolidRect(rect, m_back_color);
    m_pDC->DrawText(lpszString, rect, format);
}

void CDrawCommon::SetDrawRect(CRect rect)
{
    CRgn rgn;
    rgn.CreateRectRgnIndirect(rect);
    m_pDC->SelectClipRgn(&rgn);
}

void CDrawCommon::SetDrawRect(CDC* pDC, CRect rect)
{
    CRgn rgn;
    rgn.CreateRectRgnIndirect(rect);
    pDC->SelectClipRgn(&rgn);
}

void CDrawCommon::DrawBitmap(CBitmap& bitmap, CPoint start_point, CSize size, StretchMode stretch_mode)
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
    if (size.cx == 0 || size.cy == 0) //如果指定的size为0，则使用位图的实际大小绘制
    {
        draw_size = CSize(bm.bmWidth, bm.bmHeight);
    }
    else
    {
        draw_size = size;
        if (stretch_mode == StretchMode::FILL)
        {
            SetDrawRect(CRect(start_point, draw_size));
            float w_h_radio, w_h_radio_draw; //图像的宽高比、绘制大小的宽高比
            w_h_radio = static_cast<float>(bm.bmWidth) / bm.bmHeight;
            w_h_radio_draw = static_cast<float>(size.cx) / size.cy;
            if (w_h_radio > w_h_radio_draw) //如果图像的宽高比大于绘制区域的宽高比，则需要裁剪两边的图像
            {
                int image_width; //按比例缩放后的宽度
                image_width = bm.bmWidth * draw_size.cy / bm.bmHeight;
                start_point.x -= ((image_width - draw_size.cx) / 2);
                draw_size.cx = image_width;
            }
            else
            {
                int image_height; //按比例缩放后的高度
                image_height = bm.bmHeight * draw_size.cx / bm.bmWidth;
                start_point.y -= ((image_height - draw_size.cy) / 2);
                draw_size.cy = image_height;
            }
        }
        else if (stretch_mode == StretchMode::FIT)
        {
            draw_size = CSize(bm.bmWidth, bm.bmHeight);
            float w_h_radio, w_h_radio_draw; //图像的宽高比、绘制大小的宽高比
            w_h_radio = static_cast<float>(bm.bmWidth) / bm.bmHeight;
            w_h_radio_draw = static_cast<float>(size.cx) / size.cy;
            if (w_h_radio > w_h_radio_draw) //如果图像的宽高比大于绘制区域的宽高比
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

void CDrawCommon::DrawIcon(HICON hIcon, CPoint start_point, CSize size)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    if (size.cx == 0 || size.cy == 0)
        ::DrawIconEx(m_pDC->GetSafeHdc(), start_point.x, start_point.y, hIcon, 0, 0, 0, NULL, DI_NORMAL | DI_DEFAULTSIZE);
    else
        ::DrawIconEx(m_pDC->GetSafeHdc(), start_point.x, start_point.y, hIcon, size.cx, size.cy, 0, NULL, DI_NORMAL);
}

void CDrawCommon::BitmapStretch(CImage* pImage, CImage* ResultImage, CSize size)
{
    if (pImage->IsDIBSection())
    {
        // 取得 pImage 的 DC
        CDC* pImageDC1 = CDC::FromHandle(pImage->GetDC()); // Image 因為有自己的 DC, 所以必須使用 FromHandle 取得對應的 DC

        CBitmap* bitmap1 = pImageDC1->GetCurrentBitmap();
        BITMAP bmpInfo;
        bitmap1->GetBitmap(&bmpInfo);

        // 建立新的 CImage
        ResultImage->Create(size.cx, size.cy, bmpInfo.bmBitsPixel);
        CDC* ResultImageDC = CDC::FromHandle(ResultImage->GetDC());

        // 當 Destination 比較小的時候, 會根據 Destination DC 上的 Stretch Blt mode 決定是否要保留被刪除點的資訊
        ResultImageDC->SetStretchBltMode(HALFTONE);        // 使用最高品質的方式
        ::SetBrushOrgEx(ResultImageDC->m_hDC, 0, 0, NULL); // 調整 Brush 的起點

        // 把 pImage 畫到 ResultImage 上面
        StretchBlt(*ResultImageDC, 0, 0, size.cx, size.cy, *pImageDC1, 0, 0, pImage->GetWidth(), pImage->GetHeight(), SRCCOPY);
        // pImage->Draw(*ResultImageDC,0,0,StretchWidth,StretchHeight,0,0,pImage->GetWidth(),pImage->GetHeight());

        pImage->ReleaseDC();
        ResultImage->ReleaseDC();
    }
}

void CDrawCommon::FillRect(CRect rect, COLORREF color, BYTE alpha)
{
    m_pDC->FillSolidRect(rect, color);
}

void CDrawCommon::FillRectWithBackColor(CRect rect)
{
    m_pDC->FillSolidRect(rect, m_back_color);
}

void CDrawCommon::DrawRectOutLine(CRect rect, COLORREF color, int width, bool dot_line, BYTE alpha)
{
    CPen aPen, *pOldPen;
    aPen.CreatePen((dot_line ? PS_DOT : PS_SOLID), width, color);
    pOldPen = m_pDC->SelectObject(&aPen);
    CBrush* pOldBrush{dynamic_cast<CBrush*>(m_pDC->SelectStockObject(NULL_BRUSH))};

    rect.DeflateRect(width / 2, width / 2);
    m_pDC->Rectangle(rect);
    m_pDC->SelectObject(pOldPen);
    m_pDC->SelectObject(pOldBrush); // Restore the old brush
    aPen.DeleteObject();
}

void CDrawCommon::GetRegionFromImage(CRgn& rgn, CBitmap& cBitmap, int threshold)
{
    CDC memDC;

    memDC.CreateCompatibleDC(NULL);
    CBitmap* pOldMemBmp = NULL;
    pOldMemBmp = memDC.SelectObject(&cBitmap);

    //创建总的窗体区域，初始region为0
    rgn.CreateRectRgn(0, 0, 0, 0);

    BITMAP bit;
    cBitmap.GetBitmap(&bit); //取得位图参数，这里要用到位图的长和宽
    int y;
    for (y = 0; y < bit.bmHeight; y++)
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
    memDC.DeleteDC();
}

int CDrawCommon::GetColorBritness(COLORREF color)
{
    return (GetRValue(color) + GetGValue(color) + GetBValue(color)) / 3;
}

void CDrawCommon::DrawLine(CPoint start_point, int height, COLORREF color, BYTE alpha)
{
    CPen aPen, *pOldPen;
    aPen.CreatePen(PS_SOLID, 1, color);
    pOldPen = m_pDC->SelectObject(&aPen);
    CBrush* pOldBrush{dynamic_cast<CBrush*>(m_pDC->SelectStockObject(NULL_BRUSH))};

    m_pDC->MoveTo(start_point); //移动到起始点，默认是从下向上画
    m_pDC->LineTo(CPoint(start_point.x, start_point.y - height));
    m_pDC->SelectObject(pOldPen);
    m_pDC->SelectObject(pOldBrush); // Restore the old brush
    aPen.DeleteObject();
}

UINT DrawCommonHelper::ProccessTextFormat(CRect rect, CSize text_length, Alignment align, bool multi_line) noexcept
{
    UINT result; // CDC::DrawText()函数的文本格式
    if (multi_line)
        result = DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX;
    else
        result = DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;

    if (text_length.cx > rect.Width()) //如果文本宽度超过了矩形区域的宽度，设置了居中时左对齐
    {
        if (align == Alignment::RIGHT)
            result |= DT_RIGHT;
    }
    else
    {
        switch (align)
        {
        case Alignment::RIGHT:
            result |= DT_RIGHT;
            break;
        case Alignment::CENTER:
            result |= DT_CENTER;
            break;
        }
    }
    return result;
}

auto DrawCommonHelper::GetArgb32BitmapInfo(CRect rect) noexcept
    -> ::BITMAPINFO
{
    LONG width = std::abs(rect.Width());
    LONG height = std::abs(rect.Height());
    return GetArgb32BitmapInfo(width, height);
}

auto DrawCommonHelper::GetArgb32BitmapInfo(LONG width, LONG height) noexcept
    -> ::BITMAPINFO
{
    ::BITMAPINFO result;
    memset(&result, 0, sizeof(BITMAPINFO));
    result.bmiHeader.biSize = sizeof(result.bmiHeader);
    //保证是自上而下
    result.bmiHeader.biWidth = static_cast<LONG>(width);
    result.bmiHeader.biHeight = -static_cast<LONG>(height);
    result.bmiHeader.biPlanes = 1;
    result.bmiHeader.biBitCount = 32;
    result.bmiHeader.biCompression = BI_RGB;
    return result;
}

SizeWrapper::SizeWrapper(LONG width, LONG height)
{
    SetWidth(width);
    SetHeight(height);
}

SizeWrapper::SizeWrapper(SIZE size)
    : m_content{size} {}

SIZE* SizeWrapper::GetSizePointer()
{
    return &m_content;
}

LONG SizeWrapper::GetWidth() const noexcept
{
    return m_content.cx;
}
LONG SizeWrapper::GetHeight() const noexcept
{
    return m_content.cy;
}
void SizeWrapper::SetWidth(LONG width) noexcept
{
    m_content.cx = width;
}
void SizeWrapper::SetHeight(LONG height) noexcept
{
    m_content.cy = height;
}

D2D1DrawCommon::GdiBitmap::GdiBitmap(D2D1DrawCommon& ref_d2d1_draw_common, CRect rect)
    : m_p_bitmap{nullptr}, m_p_d2d1bitmap{nullptr},
      m_p_render_target{ref_d2d1_draw_common.m_p_support->GetWeakRenderTarget()},
      m_rect{rect}
{
    auto size = m_p_render_target->GetPixelSize();
    BITMAPINFO bitmap_info = DrawCommonHelper::GetArgb32BitmapInfo(size.width, size.height);
    m_cdc.CreateCompatibleDC(NULL);
    auto pp_bitmap_data = reinterpret_cast<void**>(&m_p_bitmap);
    m_hbitmap = ::CreateDIBSection(m_cdc, &bitmap_info, DIB_RGB_COLORS, pp_bitmap_data, NULL, 0);

    m_old_hbitmap = m_cdc.SelectObject(m_hbitmap);
    m_old_font = m_cdc.SelectObject(*ref_d2d1_draw_common.m_p_cfont);

    m_cdc.SetBkMode(TRANSPARENT);
    m_cdc.SetTextColor(ref_d2d1_draw_common.m_gdi_color);
}

D2D1DrawCommon::GdiBitmap::~GdiBitmap()
{
    //复制gdi bitmap到ID2D1Bitmap
    D2D1_BITMAP_PROPERTIES d2d1_bitmap_properties;
    d2d1_bitmap_properties.pixelFormat = m_p_render_target->GetPixelFormat();
    d2d1_bitmap_properties.dpiX = 0.f;
    d2d1_bitmap_properties.dpiY = 0.f;
    D2D1_SIZE_U bitmap_size = m_p_render_target->GetPixelSize();
    ThrowIfFailed(
        m_p_render_target->CreateBitmap(
            bitmap_size,
            m_p_bitmap,
            bitmap_size.width * 4, // RGB32（实际上是ARGB32）
            d2d1_bitmap_properties,
            &m_p_d2d1bitmap),
        "Create ID2D1Bitmap failed.");
    //绘制图片到render target
    m_p_render_target->DrawBitmap(m_p_d2d1bitmap, NULL, 1.f);
    ThrowIfFailed(m_p_render_target->Flush(), "Flush D2D1 dc render target failed when draw D2D bitmap from GDI.");
    //释放ID2D1Bitmap资源
    SAFE_RELEASE(m_p_d2d1bitmap);
    //释放GDI资源
    m_cdc.SelectObject(m_old_hbitmap);
    m_cdc.SelectObject(m_old_font);
    ::DeleteObject(m_hbitmap);
    m_cdc.DeleteDC();
}

HDC D2D1DrawCommon::GdiBitmap::GetDC()
{
    return m_cdc;
}

D2D1DrawCommon::D2D1DrawCommon()
{
}

D2D1DrawCommon::~D2D1DrawCommon()
{
    auto hr = m_p_support->GetWeakRenderTarget()->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET)
    {
        //收到此错误时，需要重新创建render target（及其创建的任何资源）。
        //此处直接重置TaskBarDlg中的m_d2d1_dc_support对象
        m_p_support->~D2D1DCSupport();
        ::new (m_p_support) D2D1DCSupport();
    }
    else
    {
        ThrowIfFailed(hr, "Error occurred when end draw.");
    }
    SAFE_RELEASE(m_p_text_format);
    SAFE_RELEASE(m_p_font);
}

void D2D1DrawCommon::Create(D2D1DCSupport& ref_support, HDC target_dc, CRect rect)
{
    m_p_support = &ref_support;
    auto* p_render_target = ref_support.GetWeakRenderTarget();
    ThrowIfFailed(p_render_target->BindDC(target_dc, &rect), "Bind dc failed.");
    ref_support.GetWeakSolidColorBrush()->SetColor({D2D1::ColorF::Black, 0.0f});
    //此调用会导致调试控制台显示形如
    // 0x00007FFAEB5E466C 处(位于 TrafficMonitor.exe 中)引发的异常: Microsoft C++ 异常: _com_error，位于内存位置 0x0000001B4A1BD8F8 处。
    //的报错。这一现象在MS的D2D GDI互操作例子中也会出现，应该是D2D的bug，不必理会，但可能会在VMware的虚拟机环境中引发崩溃。
    //此异常也无法被捕获，如果直接查看异常内存，会发现
    // const HRESULT m_hresult=S_OK; IErrorInfo* m_perrinfo=0x0000000000000000 <NULL>; wchar_t* m_pszMs=0x000001d1832bfbd0 L"輨翺";
    p_render_target->BeginDraw();
    p_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
}

void D2D1DrawCommon::SetBackColor(COLORREF back_color, BYTE alpha)
{
    auto d2d1_color_f = CRenderTarget::COLORREF_TO_D2DCOLOR(back_color, alpha);
    auto* p_weak_soild_back_color_brush = m_p_support->GetWeakSoildBackColorBrush();
    p_weak_soild_back_color_brush->SetColor(d2d1_color_f);
}

void D2D1DrawCommon::SetFont(CFont* pfont)
{
    //修改m_p_font
    LOGFONT logfont;
    {
        m_p_cfont = pfont;
        pfont->GetLogFont(&logfont);
        IDWriteFont* p_new_dwrite_font = NULL;
        m_p_support->GetWeakDWriteGdiInterop()->CreateFontFromLOGFONT(&logfont, &p_new_dwrite_font);
        SAFE_RELEASE(m_p_font);
        m_p_font = p_new_dwrite_font;
    }
    //修改m_p_text_format
    {
        IDWriteTextFormat* p_new_text_format = NULL;
        IDWriteFontFamily* p_font_family = NULL;
        ThrowIfFailed(m_p_font->GetFontFamily(&p_font_family),
                      "Get font family failed.");
        auto abs_font_height_f = static_cast<float>(std::abs(logfont.lfHeight));
        ThrowIfFailed(DWriteSupport::GetFactory()->CreateTextFormat(
                          theApp.m_taskbar_data.font.name,
                          NULL,
                          m_p_font->GetWeight(),
                          m_p_font->GetStyle(),
                          m_p_font->GetStretch(),
                          abs_font_height_f,
                          L"",
                          &p_new_text_format),
                      "Create D2D1 Text Format failed.");
        SAFE_RELEASE(m_p_text_format);
        SAFE_RELEASE(p_font_family);
        m_p_text_format = p_new_text_format;
    }
}

void D2D1DrawCommon::DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align, bool draw_back_ground, bool multi_line, BYTE alpha)
{
    auto length = ::wcslen(lpszString);
    auto length_u = static_cast<UINT>(length);
    //备份状态
    auto old_vertical_align = m_p_text_format->GetParagraphAlignment();
    auto old_horizontal_align = m_p_text_format->GetTextAlignment();
    auto old_word_warpping = m_p_text_format->GetWordWrapping();
    // GDI版本的DrawWindowText文字对齐处理没看明白
    if (multi_line)
    {
        // DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX
        m_p_text_format->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
    }
    else
    {
        // DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX
        m_p_text_format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);            // DT_SINGLELINE
        m_p_text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); // 此处仅设置竖直方向的中间对齐
    }
    switch (align)
    {
    case Alignment::LEFT:
    {
        m_p_text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        break;
    }
    case Alignment::CENTER:
    {
        m_p_text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        break;
    }
    case Alignment::RIGHT:
    {
        m_p_text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        break;
    }
    };

    auto layout_rect = Convert(rect);
    IDWriteTextLayout* p_text_layout{NULL};
    ThrowIfFailed(DWriteSupport::GetFactory()->CreateTextLayout(
                      lpszString,
                      length_u,
                      m_p_text_format,
                      layout_rect.right - layout_rect.left,
                      layout_rect.bottom - layout_rect.top,
                      &p_text_layout),
                  "Function CreateTextLayout failed.");
    DWRITE_OVERHANG_METRICS delta_size;
    p_text_layout->GetOverhangMetrics(&delta_size);
    D2D1_RECT_F rect_f{
        delta_size.left > 0 ? delta_size.left + layout_rect.left : layout_rect.left,
        delta_size.top > 0 ? delta_size.top + layout_rect.top : layout_rect.top,
        delta_size.right > 0 ? delta_size.right + layout_rect.right : layout_rect.right,
        delta_size.bottom > 0 ? delta_size.bottom + layout_rect.bottom : layout_rect.bottom};
    if (draw_back_ground)
    {
        m_p_support->GetWeakRenderTarget()->FillRectangle(rect_f, m_p_support->GetWeakSoildBackColorBrush());
    }

    SetSupporterColor(color, alpha);
    m_p_support->GetWeakRenderTarget()->DrawTextLayout(
        {layout_rect.left, layout_rect.top},
        p_text_layout,
        m_p_support->GetWeakSolidColorBrush(),
        D2D1_DRAW_TEXT_OPTIONS_NO_SNAP | D2D1_DRAW_TEXT_OPTIONS_CLIP); //不允许文字超出边界

    SAFE_RELEASE(p_text_layout);
    //恢复状态
    m_p_text_format->SetParagraphAlignment(old_vertical_align);
    m_p_text_format->SetTextAlignment(old_horizontal_align);
    m_p_text_format->SetWordWrapping(old_word_warpping);
}

void D2D1DrawCommon::FillRect(CRect rect, COLORREF color, BYTE alpha)
{
    SetSupporterColor(color, alpha);
    auto rect_f = Convert(rect);
    m_p_support->GetWeakRenderTarget()->FillRectangle(rect_f, m_p_support->GetWeakSolidColorBrush());
}

void D2D1DrawCommon::DrawRectOutLine(CRect rect, COLORREF color, int width, bool dot_line, BYTE alpha)
{
    rect.DeflateRect(width / 2, width / 2);
    auto rect_f = Convert(rect);
    SetSupporterColor(color, alpha);
    auto width_f = static_cast<float>(width);
    if (dot_line)
    {
        m_p_support->GetWeakRenderTarget()->DrawRectangle(
            rect_f,
            m_p_support->GetWeakSolidColorBrush(),
            width_f,
            m_p_support->GetWeakPsDotStyle());
    }
    else
    {
        m_p_support->GetWeakRenderTarget()->DrawRectangle(
            rect_f,
            m_p_support->GetWeakSolidColorBrush(),
            width_f);
    }
}

void D2D1DrawCommon::DrawLine(CPoint start_point, int height, COLORREF color, BYTE alpha)
{
    SetSupporterColor(color, alpha);
    // m_pDC->MoveTo(start_point); //移动到起始点，默认是从下向上画
    // m_pDC->LineTo(CPoint(start_point.x, start_point.y - height));
    auto d2d1_start_point = Convert(start_point);
    D2D1_POINT_2F d2d1_end_point{d2d1_start_point.x, d2d1_start_point.y - height};
    m_p_support->GetWeakRenderTarget()->DrawLine(d2d1_start_point, d2d1_end_point, m_p_support->GetWeakSolidColorBrush());
}

void D2D1DrawCommon::SetTextColor(const COLORREF color, BYTE alpha)
{
    m_gdi_color = color;
    SetSupporterColor(color, alpha);
}

void D2D1DrawCommon::SetSupporterColor(const COLORREF color, BYTE alpha)
{
    auto d2d1_color_f = CRenderTarget::COLORREF_TO_D2DCOLOR(color, alpha);
    auto* weak_solid_color_brush = m_p_support->GetWeakSolidColorBrush();
    weak_solid_color_brush->SetColor(d2d1_color_f);
}

D2D1_RECT_F D2D1DrawCommon::Convert(CRect rect)
{
    D2D1_RECT_F result{
        static_cast<float>(rect.left),
        static_cast<float>(rect.top),
        static_cast<float>(rect.right),
        static_cast<float>(rect.bottom)};

    return result;
}

D2D1_POINT_2F D2D1DrawCommon::Convert(CPoint point)
{
    D2D1_POINT_2F result{
        static_cast<float>(point.x),
        static_cast<float>(point.y)};
    return result;
}

DrawCommonBuffer::DrawCommonBuffer(HWND hwnd, CRect rect)
    : m_p_display_bitmap{nullptr}, m_target_hwnd{hwnd}
{
    m_size.SetWidth(std::abs(rect.Width()));
    m_size.SetHeight(std::abs(rect.Height()));

    BITMAPINFO bitmap_info = DrawCommonHelper::GetArgb32BitmapInfo(rect);
    auto pp_bitmap_for_show_data = reinterpret_cast<void**>(&m_p_display_bitmap);
    m_mem_display_dc.CreateCompatibleDC(NULL);
    m_display_hbitmap = ::CreateDIBSection(m_mem_display_dc, &bitmap_info, DIB_RGB_COLORS, pp_bitmap_for_show_data, NULL, 0);
    m_old_display_bitmap = m_mem_display_dc.SelectObject(m_display_hbitmap);
}

DrawCommonBuffer::~DrawCommonBuffer()
{
    POINT start_location{0, 0};
    UPDATELAYEREDWINDOWINFO update_window_info;
    ::memset(&update_window_info, 0, sizeof(UPDATELAYEREDWINDOWINFO));
    update_window_info.cbSize = sizeof(UPDATELAYEREDWINDOWINFO);
    // update_window_info.hdcDst = NULL;
    // update_window_info.pptDst = NULL; 不更新窗口位置
    update_window_info.psize = m_size.GetSizePointer();
    update_window_info.hdcSrc = m_mem_display_dc;
    update_window_info.pptSrc = &start_location;
    // update_window_info.crKey = 0;
    update_window_info.pblend = GetDefaultBlendFunctionPointer();
    update_window_info.dwFlags = ULW_ALPHA;
    // m_update_window_info.prcDirty = NULL;
    BOOL state = ::UpdateLayeredWindowIndirect(m_target_hwnd, &update_window_info);
    if (state == 0)
    {
        auto error_code = ::GetLastError();
        //写入错误日志
        CString error_info{};
        error_info.Format(_T("Call UpdateLayeredWindowIndirect failed. Use GDI render instead. Error code = %ld."), error_code);
        CCommon::WriteLog(error_info, theApp.m_log_path.c_str());
        //写入系统格式化后的错误信息
        if (error_code != 0)
        {
            LPTSTR fomat_error = nullptr;
            ::FormatMessage(
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                error_code,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                reinterpret_cast<decltype(fomat_error)>(&fomat_error),
                0,
                NULL);
            if (fomat_error != nullptr)
            {
                CCommon::WriteLog(fomat_error, theApp.m_log_path.c_str());
                ::LocalFree(fomat_error);
            }
        }
        //禁用D2D
        theApp.m_taskbar_data.disable_d2d = true;
        //展示错误信息
        ::MessageBox(NULL, CCommon::LoadText(IDS_UPDATE_TASKBARDLG_FAILED_TIP), NULL, MB_OK | MB_ICONWARNING);
    }

    m_mem_display_dc.SelectObject(m_old_display_bitmap);
    ::DeleteObject(m_display_hbitmap);
    m_mem_display_dc.DeleteDC();
}

auto DrawCommonBuffer::GetDefaultBlendFunctionPointer()
    -> const ::PBLENDFUNCTION
{
    static ::BLENDFUNCTION result{
        AC_SRC_OVER,
        0,
        0xFF,
        AC_SRC_ALPHA};
    return &result;
}

CDC* DrawCommonBuffer::GetMemDC()
{
    return &m_mem_display_dc;
}

HDC DrawCommonBuffer::GetDC()
{
    return m_mem_display_dc;
}

DrawCommonBufferUnion::~DrawCommonBufferUnion()
{
    switch (m_type)
    {
    case DrawCommonHelper::RenderType::Default:
    {
        m_content.cdraw_double_buffer.~Nullable();
        break;
    }
    case DrawCommonHelper::RenderType::D2D1:
    {
        m_content.draw_common_buffer.~Nullable();
        break;
    }
    };
}

IDrawBuffer& DrawCommonBufferUnion::Get()
{
    switch (m_type)
    {
    case DrawCommonHelper::RenderType::Default:
    {
        return m_content.cdraw_double_buffer.Get();
        break;
    }
    case DrawCommonHelper::RenderType::D2D1:
    {
        return m_content.draw_common_buffer.Get();
        break;
    }
    default:
    {
        throw std::runtime_error{"No matching render buffer."};
    }
    };
}

DrawCommonUnion::DrawCommonUnion(DrawCommonHelper::RenderType type)
    : m_type{type}
{
    switch (type)
    {
    case DrawCommonHelper::RenderType::Default:
    {
        ::new (&m_content.cdraw_common) Nullable<CDrawCommon>();
        m_content.cdraw_common.Construct();
        break;
    }
    case DrawCommonHelper::RenderType::D2D1:
    {
        ::new (&m_content.cdraw_common) Nullable<D2D1DrawCommon>();
        m_content.d2d1_draw_common.Construct();
        break;
    }
    default:
    {
        break;
    }
    }
}

DrawCommonUnion::~DrawCommonUnion()
{
    switch (m_type)
    {
    case DrawCommonHelper::RenderType::Default:
    {
        m_content.cdraw_common.~Nullable();
        break;
    }
    case DrawCommonHelper::RenderType::D2D1:
    {
        m_content.d2d1_draw_common.~Nullable();
        break;
    }
    };
}

CDrawCommon& DrawCommonUnion::Get(DrawCommonHelper::RenderTypeDefaultTag) noexcept
{
    return m_content.cdraw_common.Get();
}

D2D1DrawCommon& DrawCommonUnion::Get(DrawCommonHelper::RenderTypeD2D1Tag) noexcept
{
    return m_content.d2d1_draw_common.Get();
}
