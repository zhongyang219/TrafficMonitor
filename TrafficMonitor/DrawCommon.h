//封装的绘图类
#pragma once
#include "IDrawCommon.h"
#include "D2D1Support.h"
#include "CommonData.h"
#include "Nullable.hpp"

class CDrawCommon final : public IDrawCommon
{
public:

    CDrawCommon();
    ~CDrawCommon();

    void Create(CDC* pDC, CWnd* pMainWnd);
    void SetFont(CFont* pfont) override;		//设置绘制文本的字体
    void SetDC(CDC* pDC);		//设置绘图的DC
    virtual CDC* GetDC() override { return m_pDC; }
    void SetBackColor(COLORREF back_color, BYTE alpha = 255) override { m_back_color = back_color; }
    void SetTextColor(const COLORREF text_color, BYTE alpha = 255) override
    {
        m_pDC->SetTextColor(text_color);
    }

    void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align = Alignment::LEFT, bool draw_back_ground = false, bool multi_line = false, BYTE alpha = 255) override; //在指定的矩形区域内绘制文本

    void SetDrawRect(CRect rect) override;		//设置绘图剪辑区域
    static void SetDrawRect(CDC* pDC, CRect rect);

    //绘制一个位图
    //（注意：当stretch_mode设置为StretchMode::FILL（填充）时，会设置绘图剪辑区域，如果之后需要绘制其他图形，
    //需要重新设置绘图剪辑区域，否则图片外的区域会无法绘制）
    void DrawBitmap(CBitmap& bitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH);
    void DrawBitmap(UINT bitmap_id, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH);
    void DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH, BYTE alpha = 255) override;

    void DrawIcon(HICON hIcon, CPoint start_point, CSize size);

    //将图片拉伸到指定尺寸(https://blog.csdn.net/sichuanpb/article/details/22986877)
    static void BitmapStretch(CImage* pImage, CImage* ResultImage, CSize size);

    void FillRect(CRect rect, COLORREF color, BYTE alpha = 255) override; //用纯色填充矩形
    void FillRectWithBackColor(CRect rect);			//使用背景色填充矩形
    void DrawRectOutLine(CRect rect, COLORREF color, int width = 1, bool dot_line = false, BYTE alpha = 255) override; //绘制矩形边框。如果dot_line为true，则为虚线

    //从图像创建区域，如果像素点的亮度小于threshold（取值为0~255，0为黑色，255为白色），则该像素点在区域外
    //https://blog.csdn.net/tajon1226/article/details/6589180
    static void GetRegionFromImage(CRgn& rgn, CBitmap& cBitmap, int threshold);

    void DrawLine(CPoint start_point, int height, COLORREF color, BYTE alpha = 255) override; //使用当前画笔画线

    virtual int GetTextWidth(LPCTSTR lpszString) override;

private:
    CDC* m_pDC{};		//用于绘图的CDC类的指针
    CWnd* m_pMainWnd{};	//绘图窗口的句柄
    CFont* m_pfont{};
    COLORREF m_back_color{};

    static int GetColorBritness(COLORREF color);
};


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
    CDC* m_pDC;
    CDC m_memDC;
    CBitmap m_memBitmap;
    CBitmap* m_pOldBit;
    CRect m_rect;
};

namespace DrawCommonHelper
{
    UINT ProccessTextFormat(CRect rect, CSize text_length, Alignment align, bool multi_line) noexcept;

    //根据图片拉伸模式，计算绘制图片的实际位置
    //image_size[int]：图片的原始大小
    //start_point[int][out]：绘制区域的起始位置
    //size[int][out]：绘制区域的大小
    //stretch_mode[int]：拉伸模式
    void ImageDrawAreaConvert(CSize image_size, CPoint& start_point, CSize& size, IDrawCommon::StretchMode stretch_mode);

    //修正位图中文本部分的Alpha通道
    //使用了UpdateLayeredWindow后，使用GDI绘制的文本也会变得透明，此函数会遍历bitmap中alpha值为0的部分，将其修正为正确的alpha值
    void FixBitmapTextAlpha(HBITMAP hBitmap, BYTE alpha, const std::vector<CRect>& rects);
};
