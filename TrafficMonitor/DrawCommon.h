//封装的绘图类
#pragma once
#include "D2D1Support.h"
#include "CommonData.h"
#include "Nullable.hpp"

class IDrawBuffer
{
public:
    virtual CDC *GetMemDC() = 0;
protected:
    ~IDrawBuffer() = default;
};

class IDrawCommon
{
public:
    virtual void SetBackColor(COLORREF back_color, BYTE alpha = 255) = 0;
    //设置绘制文本的字体
    virtual void SetFont(CFont* pfont) = 0;
    //在指定的矩形区域内绘制文本
    virtual void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align = Alignment::LEFT, bool draw_back_ground = false, bool multi_line = false, BYTE alpha = 255) = 0;
    //用纯色填充矩形
    virtual void FillRect(CRect rect, COLORREF color, BYTE alpha = 255) = 0;
    //绘制矩形边框。如果dot_line为true，则为虚线
    virtual void DrawRectOutLine(CRect rect, COLORREF color, int width = 1, bool dot_line = false, BYTE alpha = 255) = 0;
    //使用当前画笔画线
    virtual void DrawLine(CPoint start_point, int height, COLORREF color, BYTE alpha = 255) = 0;
    virtual void SetTextColor(const COLORREF color, BYTE alpha = 255) = 0;

protected:
    ~IDrawCommon() = default;
};
class CDrawCommon final : public IDrawCommon
{
public:

    //拉伸模式
    enum class StretchMode
    {
        STRETCH,		//拉伸，会改变比例
        FILL,			//填充，不改变比例，会裁剪长边
        FIT			//适应，不会改变比例，不裁剪
    };

    CDrawCommon();
    ~CDrawCommon();

    void Create(CDC* pDC, CWnd* pMainWnd);
    void SetFont(CFont* pfont) override;		//设置绘制文本的字体
    void SetDC(CDC* pDC);		//设置绘图的DC
    CDC* GetDC() { return m_pDC; }
    void SetBackColor(COLORREF back_color, BYTE alpha = 255) override { m_back_color = back_color; }
    void SetTextColor(const COLORREF text_color, BYTE alpha = 255) override
    {
        m_pDC->SetTextColor(text_color);
    }

    void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align = Alignment::LEFT, bool draw_back_ground = false, bool multi_line = false, BYTE alpha = 255) override; //在指定的矩形区域内绘制文本

    void SetDrawRect(CRect rect);		//设置绘图剪辑区域
    static void SetDrawRect(CDC* pDC, CRect rect);

    //绘制一个位图
    //（注意：当stretch_mode设置为StretchMode::FILL（填充）时，会设置绘图剪辑区域，如果之后需要绘制其他图形，
    //需要重新设置绘图剪辑区域，否则图片外的区域会无法绘制）
    void DrawBitmap(CBitmap& bitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH);
    void DrawBitmap(UINT bitmap_id, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH);
    void DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH);

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

    CDC* GetMemDC() override
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
    constexpr static BYTE GDI_NO_MODIFIED_FLAG = 0x01;
    constexpr static BYTE OPAQUE_ALPHA_VALUE = 0xFF;
    constexpr static BYTE TRANSPARENT_ALPHA_VALUE = 0x00;
    constexpr static BYTE AVAILABLE_MINIMUM_ALPHA = 0x02;
    constexpr static BYTE GDI_MODIFIED_FLAG = 0x00;

    template <class Func>
    void ForEachPixelInBitmapForDraw(BYTE* p_data, std::size_t x_width, std::size_t x_start, std::size_t x_end, std::size_t y_start, std::size_t y_end, Func func)
    {
        auto distance = x_width * (y_end - y_start) + x_start;
        std::advance(p_data, distance);
        for (std::size_t y = y_start; y < y_end; y++)
        {
            for (std::size_t x = x_start; x < x_end; x++)
            {
                func(p_data);
                std::advance(p_data, 4);
            }
        }
    }
    UINT ProccessTextFormat(CRect rect, CSize text_length, Alignment align, bool multi_line) noexcept;
    auto GetArgb32BitmapInfo(CRect rect) noexcept
        -> ::BITMAPINFO;
    auto GetArgb32BitmapInfo(LONG width, LONG height) noexcept
        -> ::BITMAPINFO;

    enum class RenderType
    {
        //使用GDI
        Default,
        //如果支持，使用D2D1
        D2D1
    };
    template <RenderType render_type>
    struct RenderTypeTag
    {
        constexpr static RenderType type = render_type;
    };
    using RenderTypeDefaultTag = RenderTypeTag<RenderType::Default>;
    using RenderTypeD2D1Tag = RenderTypeTag<RenderType::D2D1>;
};

class SizeWrapper
{
private:
    SIZE m_content;

public:
    SizeWrapper(LONG width = 0, LONG height = 0);
    SizeWrapper(SIZE size);
    ~SizeWrapper() = default;

    SIZE* GetSizePointer();

    LONG GetWidth() const noexcept;
    LONG GetHeight() const noexcept;
    void SetWidth(LONG width) noexcept;
    void SetHeight(LONG height) noexcept;
};

class D2D1DrawCommon final : public IDrawCommon
{
private:
    D2D1DCSupport* m_p_support{NULL};
    IDWriteTextFormat* m_p_text_format{NULL};
    IDWriteFont* m_p_font{NULL};
    CFont* m_p_cfont{NULL};
    COLORREF m_gdi_color{};

    class GdiBitmap
    {
    private:
        CDC m_cdc;
        BYTE* m_p_bitmap;
        ID2D1Bitmap* m_p_d2d1bitmap;
        ID2D1DCRenderTarget* m_p_render_target;
        HBITMAP m_hbitmap;
        HGDIOBJ m_old_hbitmap;
        HGDIOBJ m_old_font;
        CRect m_rect;

    public:
        /**
         * @brief 初始化 GdiBitmap，会自动根据render_traget创建一个RGB32的内存图片，D2D可以处理alpha通道
         * @param ref_d2d1_draw_common 关联的D2D1DrawCommon对象，借助其中的信息初始化自身
         * @param draw_rect 进行alpha通道处理的区域
         */
        GdiBitmap(D2D1DrawCommon& ref_d2d1_draw_common, CRect draw_rect);
        ~GdiBitmap();
        HDC GetDC();

    private:
    };

public:
    D2D1DrawCommon();
    ~D2D1DrawCommon();
    void Create(D2D1DCSupport& ref_support, HDC target_dc, CRect rect);
    template <class GdiOp>
    void ExecuteGdiOperation(CRect rect, GdiOp gdi_op)
    {
        auto* weak_render_target = m_p_support->GetWeakRenderTarget();
        GdiBitmap gdi_wrapper{*this, rect};
        gdi_op(gdi_wrapper.GetDC());
    }

    void SetBackColor(COLORREF back_color, BYTE apha = 255) override;
    void SetFont(CFont* pfont) override;
    //在指定的矩形区域内绘制文本
    void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align = Alignment::LEFT, bool draw_back_ground = false, bool multi_line = false, BYTE alpha = 255) override;
    //用纯色填充矩形
    void FillRect(CRect rect, COLORREF color, BYTE alpha = 255) override;
    //绘制矩形边框。如果dot_line为true，则为虚线
    void DrawRectOutLine(CRect rect, COLORREF color, int width = 1, bool dot_line = false, BYTE alpha = 255) override;
    //使用当前画笔画线
    void DrawLine(CPoint start_point, int height, COLORREF color, BYTE alpha = 255) override;
    void SetTextColor(const COLORREF color, BYTE alpha = 255) override;

private:
    void SetSupporterColor(const COLORREF color, BYTE alpha);
    constexpr static float DEFAULT_DPI = 96.f;
    static D2D1_RECT_F Convert(CRect rect);
    static D2D1_POINT_2F Convert(CPoint point);
};

class DrawCommonBuffer final : public IDrawBuffer
{
public:
    // reference from https://www.cnblogs.com/setoutsoft/p/4086051.html
    DrawCommonBuffer(HWND hwnd, CRect rect);
    // referce from https://www.cnblogs.com/strive-sun/p/13073015.html
    ~DrawCommonBuffer();
    DrawCommonBuffer(const DrawCommonBuffer&) = delete;
    DrawCommonBuffer operator=(const DrawCommonBuffer&) = delete;

    CDC* GetMemDC() override;
    HDC GetDC();

private:
    static auto GetDefaultBlendFunctionPointer()
        -> const ::PBLENDFUNCTION;

    CDC m_mem_display_dc;
    HBITMAP m_display_hbitmap;
    BYTE* m_p_display_bitmap;
    HGDIOBJ m_old_display_bitmap;
    HWND m_target_hwnd;
    SizeWrapper m_size;
};

class DrawCommonBufferUnion
{
private:
    const DrawCommonHelper::RenderType m_type;
    union Content
    {
        Content(){};  //手工管理构造
        ~Content(){}; //手工管理析构
        Nullable<CDrawDoubleBuffer> cdraw_double_buffer;
        Nullable<DrawCommonBuffer> draw_common_buffer;
    } m_content;

public:
    DrawCommonBufferUnion(DrawCommonHelper::RenderType type)
        : m_type{type} {};
    template <class... Args>
    void CreateDefaultVerion(Args&&... args)
    {
        ::new (&m_content.cdraw_double_buffer) Nullable<CDrawDoubleBuffer>();
        m_content.cdraw_double_buffer.Construct(std::forward<Args>(args)...);
    }
    template <class... Args>
    void CreateD2D1Version(Args&&... args)
    {
        ::new (&m_content.draw_common_buffer) Nullable<DrawCommonBuffer>();
        m_content.draw_common_buffer.Construct(std::forward<Args>(args)...);
    }
    ~DrawCommonBufferUnion();

    IDrawBuffer& Get();
};

class DrawCommonUnion
{
private:
    const DrawCommonHelper::RenderType m_type;
    union Content
    {
        Content(){};  //手工管理构造
        ~Content(){}; //手工管理析构
        Nullable<CDrawCommon> cdraw_common;
        Nullable<D2D1DrawCommon> d2d1_draw_common;
    } m_content;

public:
    DrawCommonUnion(DrawCommonHelper::RenderType type);
    ~DrawCommonUnion();

    CDrawCommon& Get(DrawCommonHelper::RenderTypeDefaultTag) noexcept;
    D2D1DrawCommon& Get(DrawCommonHelper::RenderTypeD2D1Tag) noexcept;
};