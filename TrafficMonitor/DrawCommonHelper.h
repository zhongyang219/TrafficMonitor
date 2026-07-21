#pragma once
#include "IDrawCommon.h"

//用于双缓冲绘图的类
class CDrawDoubleBuffer final : public IDrawBuffer
{
public:
    CDrawDoubleBuffer(CDC* pDC, CRect rect)
        : m_pDC(pDC), m_rect(rect)
    {
        if (m_pDC != nullptr)
        {
            m_memDC.CreateCompatibleDC(NULL);
            m_memBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
            m_pOldBit = m_memDC.SelectObject(&m_memBitmap);
        }
    }

    ~CDrawDoubleBuffer()
    {
        if (m_pDC != nullptr)
        {
            m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), &m_memDC, 0, 0, SRCCOPY);
            m_memDC.SelectObject(m_pOldBit);
            m_memBitmap.DeleteObject();
            m_memDC.DeleteDC();
        }
    }

    CDC* GetMemDC()
    {
        return &m_memDC;
    }

private:
    CDC* m_pDC{};
    CDC m_memDC;
    CBitmap m_memBitmap;
    CBitmap* m_pOldBit{};
    CRect m_rect;
};

namespace DrawCommonHelper
{
    UINT ProccessTextFormat(CRect rect, CSize text_length, IDrawCommon::Alignment align, bool multi_line) noexcept;

    //根据图片拉伸模式，计算绘制图片的实际位置
    //image_size[int]：图片的原始大小
    //start_point[int][out]：绘制区域的起始位置
    //size[int][out]：绘制区域的大小
    //stretch_mode[int]：拉伸模式
    void ImageDrawAreaConvert(CSize image_size, CPoint& start_point, CSize& size, IDrawCommon::StretchMode stretch_mode);

    struct Point
    {
    public:
        Point() {}
        Point(int x, int y)
            : m_x(x), m_y(y)
        {}
        bool operator==(const Point& a) const
        {
            return m_x == a.m_x && m_y == a.m_y;
        }
        bool operator<(const Point& a) const
        {
            if (m_x == a.m_x)
                return m_y < a.m_y;
            else
                return m_x < a.m_x;
        }

    private:
        int m_x{};
        int m_y{};
    };

    //获取一个位置中完全透明的点，并保存到points中
    void GetBitmapAlphaPixel(HBITMAP hBitmap, std::set<Point>& points);

    //修正位图中文本部分的Alpha通道
    //使用了UpdateLayeredWindow后，使用GDI绘制的文本也会变得透明，此函数会遍历bitmap中alpha值为0，但是不在alpha_points中的像素，将其修正为正确的alpha值
    void FixBitmapTextAlpha(HBITMAP hBitmap, BYTE alpha, std::set<Point> alpha_points);
};

