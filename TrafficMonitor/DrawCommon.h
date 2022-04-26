//封装的绘图类
#pragma once
#include "D2D1Support.h"
#include "CommonData.h"

class IDrawBuffer
{
public:
    virtual CDC *GetMemDC() = 0;
protected:
    ~IDrawBuffer() = default;
};

class CDrawCommon
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
    void SetFont(CFont* pfont);		//设置绘制文本的字体
    void SetDC(CDC* pDC);		//设置绘图的DC
    CDC* GetDC() { return m_pDC; }
    void SetBackColor(COLORREF back_color) { m_back_color = back_color; }

    void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align = Alignment::LEFT, bool draw_back_ground = false, bool multi_line = false);	//在指定的矩形区域内绘制文本

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

    void FillRect(CRect rect, COLORREF color);		//用纯色填充矩形
    void FillRectWithBackColor(CRect rect);			//使用背景色填充矩形
    void DrawRectOutLine(CRect rect, COLORREF color, int width = 1, bool dot_line = false);	//绘制矩形边框。如果dot_line为true，则为虚线

    //从图像创建区域，如果像素点的亮度小于threshold（取值为0~255，0为黑色，255为白色），则该像素点在区域外
    //https://blog.csdn.net/tajon1226/article/details/6589180
    static void GetRegionFromImage(CRgn& rgn, CBitmap& cBitmap, int threshold);

    void DrawLine(CPoint start_point, int height, COLORREF color);  //使用当前画笔画线

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
    constexpr static BYTE GDI_NO_MODIFIED_FLAG = 0xFF;
    constexpr static BYTE OPAQUE_ALPHA_VALUE = 0xFF;
    constexpr static BYTE AVAILABLE_MINIMUM_ALPHA = 0x01;
    constexpr static BYTE GDI_MODIFIED_FLAG = 0x00;

    template <class Func>
    void ForEachPixelInBitmapForDraw(BYTE* p_data,std::size_t x_start, std::size_t x_end, std::size_t y_start, std::size_t y_end, Func func)
    {
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
        Transparent
    };
    template<RenderType render_type>
    struct RenderTypeTag
    {
        constexpr static RenderType type = render_type;
    };
    using RenderTypeDefaultTag = RenderTypeTag<RenderType::Default>;
    using RenderTypeTransparentTag = RenderTypeTag<RenderType::Transparent>;
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

class DrawCommonBuffer;

class D2D1DrawCommon
{
private:
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> m_p_render_target;
    class GdiBitmap
    {
    private:
        CDC m_cdc;
        BYTE* m_p_bitmap;
        ID2D1Bitmap* m_p_d2d1bitmap;
        Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> m_p_render_target;
        HBITMAP m_hbitmap;
        HGDIOBJ m_old_hbitmap;
        UINT32 m_width;
        UINT32 m_height;

    public:
      GdiBitmap(D2D1_SIZE_U size, Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> p_render_target);
      ~GdiBitmap();
      HDC GetDC();
    };

public:
    D2D1DrawCommon();
    ~D2D1DrawCommon();
    void Create(D2D1DCSupport& ref_support, HDC target_dc, CRect rect);
    template <class GdiOp>
    void ExecuteGdiOperation(GdiOp gdi_op)
    {
        GdiBitmap gdi_wrapper{m_p_render_target->GetPixelSize(), m_p_render_target};
        gdi_op(gdi_wrapper.GetDC());
    }
private:
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
    UPDATELAYEREDWINDOWINFO m_update_window_info;
};

class DrawCommonBufferUnion
{
private:
    const DrawCommonHelper::RenderType m_type;
    union Content {
      Content(){};  //手工管理构造
      ~Content(){}; //手工管理析构
      CDrawDoubleBuffer cdraw_double_buffer;
      DrawCommonBuffer draw_common_buffer;
    } m_content;

public:
    template<class... Args>
    DrawCommonBufferUnion(DrawCommonHelper::RenderType type, Args&&... args)
        :m_type{type}
    {
        switch (m_type)
        {
        case DrawCommonHelper::RenderType::Default:
        {
            ::new (&m_content.cdraw_double_buffer) CDrawDoubleBuffer(std::forward<Args>(args)...);
            break;
        }
        case DrawCommonHelper::RenderType::Transparent:
        {
            ::new (&m_content.draw_common_buffer) DrawCommonBuffer(std::forward<Args>(args)...);
            break;
        }
        };
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
        CDrawCommon cdraw_common;
        D2D1DrawCommon d2d1_draw_common;
    } m_content;

public:
    template <class... Args>
    DrawCommonUnion(DrawCommonHelper::RenderType type, Args&&... args)
        : m_type{type}
    {
        switch (m_type)
        {
        case DrawCommonHelper::RenderType::Default:
        {
            ::new (&m_content.cdraw_common) CDrawDoubleBuffer(std::forward<Args>(args)...);
            break;
        }
        case DrawCommonHelper::RenderType::Transparent:
        {
            ::new (&m_content.d2d1_draw_common) DrawCommonBuffer(std::forward<Args>(args)...);
            break;
        }
        };
    }
    ~DrawCommonUnion();

    CDrawCommon& Get(DrawCommonHelper::RenderTypeDefaultTag)noexcept;
    D2D1DrawCommon& Get(DrawCommonHelper::RenderTypeTransparentTag)noexcept;
};