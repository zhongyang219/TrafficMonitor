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

    DrawCommonHelper::ImageDrawAreaConvert(CSize(bm.bmWidth, bm.bmHeight), start_point, size, stretch_mode);

    m_pDC->StretchBlt(start_point.x, start_point.y, size.cx, size.cy, &memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    memDC.DeleteDC();
}

void CDrawCommon::DrawBitmap(UINT bitmap_id, CPoint start_point, CSize size, StretchMode stretch_mode)
{
    CBitmap bitmap;
    bitmap.LoadBitmap(bitmap_id);
    DrawBitmap(bitmap, start_point, size, stretch_mode);
}

void CDrawCommon::DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode, BYTE)
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

int CDrawCommon::GetTextWidth(LPCTSTR lpszString)
{
    return m_pDC->GetTextExtent(lpszString).cx;
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

void DrawCommonHelper::ImageDrawAreaConvert(CSize image_size, CPoint& start_point, CSize& size, IDrawCommon::StretchMode stretch_mode)
{
    if (size.cx == 0 || size.cy == 0)       //如果指定的size为0，则使用位图的实际大小绘制
    {
        size = CSize(image_size.cx, image_size.cy);
    }
    else
    {
        if (stretch_mode == IDrawCommon::StretchMode::FILL)
        {
            float w_h_ratio, w_h_ratio_draw;        //图像的宽高比、绘制大小的宽高比
            w_h_ratio = static_cast<float>(image_size.cx) / image_size.cy;
            w_h_ratio_draw = static_cast<float>(size.cx) / size.cy;
            if (w_h_ratio > w_h_ratio_draw)     //如果图像的宽高比大于绘制区域的宽高比，则需要裁剪两边的图像
            {
                int image_width;        //按比例缩放后的宽度
                image_width = image_size.cx * size.cy / image_size.cy;
                start_point.x -= ((image_width - size.cx) / 2);
                size.cx = image_width;
            }
            else
            {
                int image_height;       //按比例缩放后的高度
                image_height = image_size.cy * size.cx / image_size.cx;
                start_point.y -= ((image_height - size.cy) / 2);
                size.cy = image_height;
            }
        }
        else if (stretch_mode == IDrawCommon::StretchMode::FIT)
        {
            CSize draw_size = image_size;
            float w_h_ratio, w_h_ratio_draw;        //图像的宽高比、绘制大小的宽高比
            w_h_ratio = static_cast<float>(image_size.cx) / image_size.cy;
            w_h_ratio_draw = static_cast<float>(size.cx) / size.cy;
            if (w_h_ratio > w_h_ratio_draw)     //如果图像的宽高比大于绘制区域的宽高比
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
            size = draw_size;
        }
    }
}

void DrawCommonHelper::FixBitmapTextAlpha(HBITMAP hBitmap, BYTE alpha, const std::vector<CRect>& rects)
{
    if (rects.empty())
        return;
    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);

    int width = bm.bmWidth;
    int height = bm.bmHeight;

    // 获取位图的像素数据
    BITMAPINFO bmpInfo = { 0 };
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = width;
    bmpInfo.bmiHeader.biHeight = -height; // top-down DIB
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    HDC hdc = CreateCompatibleDC(NULL);
    SelectObject(hdc, hBitmap);

    // 分配内存存储位图像素
    RGBQUAD* pPixels = new RGBQUAD[width * height];
    GetDIBits(hdc, hBitmap, 0, height, pPixels, &bmpInfo, DIB_RGB_COLORS);

    // 遍历所有矩形区域
    for (const auto& rect : rects)
    {
        int startX = max(0, rect.left);
        int startY = max(0, rect.top);
        int endX = min(width, rect.right);
        int endY = min(height, rect.bottom);

        // 遍历当前矩形内的像素
        for (int y = startY; y < endY; ++y)
        {
            for (int x = startX; x < endX; ++x)
            {
                int index = y * width + x;
                //如果检测到alpha值为0，则可能是被错误地设置成透明的文本部分，将其修正为正确的alpha值
                if (pPixels[index].rgbReserved == 0)
                    pPixels[index].rgbReserved = alpha; // 设置Alpha通道
            }
        }
    }

    // 将修改后的像素数据写回位图
    SetDIBits(hdc, hBitmap, 0, height, pPixels, &bmpInfo, DIB_RGB_COLORS);

    delete[] pPixels;
    DeleteDC(hdc);
}
