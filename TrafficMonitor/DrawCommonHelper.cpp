#include "stdafx.h"
#include "DrawCommonHelper.h"

UINT DrawCommonHelper::ProccessTextFormat(CRect rect, CSize text_length, IDrawCommon::Alignment align, bool multi_line) noexcept
{
    UINT result; // CDC::DrawText()函数的文本格式
    if (multi_line)
        result = DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX;
    else
        result = DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;

    if (text_length.cx > rect.Width()) //如果文本宽度超过了矩形区域的宽度，设置了居中时左对齐
    {
        if (align == IDrawCommon::Alignment::RIGHT)
            result |= DT_RIGHT;
    }
    else
    {
        switch (align)
        {
        case IDrawCommon::Alignment::RIGHT:
            result |= DT_RIGHT;
            break;
        case IDrawCommon::Alignment::CENTER:
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

void DrawCommonHelper::GetBitmapAlphaPixel(HBITMAP hBitmap, std::set<Point>& points)
{
    points.clear();
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

    // 遍历所有像素点
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int index = y * width + x;
            //添加alpha值为0的像素点
            if (pPixels[index].rgbReserved == 0)
                points.insert(Point(x, y));
        }
    }

    delete[] pPixels;
    DeleteDC(hdc);

}

void DrawCommonHelper::FixBitmapTextAlpha(HBITMAP hBitmap, BYTE alpha, std::set<Point> alpha_points)
{
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

    // 遍历所有像素
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int index = y * width + x;
            //如果检测到alpha值为0，但是却不在alpha_points里，将其修正为正确的alpha值
            if (pPixels[index].rgbReserved == 0 && !alpha_points.contains(Point(x, y)))
                pPixels[index].rgbReserved = alpha; // 设置Alpha通道
        }
    }

    // 将修改后的像素数据写回位图
    SetDIBits(hdc, hBitmap, 0, height, pPixels, &bmpInfo, DIB_RGB_COLORS);

    delete[] pPixels;
    DeleteDC(hdc);
}
