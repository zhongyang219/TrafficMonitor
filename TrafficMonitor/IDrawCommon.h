#pragma once
#include "CommonData.h"
#include "PluginInterface.h"

class IDrawBuffer
{
public:
    virtual ~IDrawBuffer() = default;
};

class IDrawCommon : public IPluginDrawer
{
public:

    virtual void SetBackColor(COLORREF back_color, BYTE alpha = 255) = 0;
    // 设置绘制文本的字体
    virtual void SetFont(CFont* pfont) = 0;
    // 在指定的矩形区域内绘制文本
    virtual void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align = Alignment::LEFT, bool draw_back_ground = false, bool multi_line = false, BYTE alpha = 255) = 0;
    // 设置绘图剪辑区域
    virtual void SetDrawRect(CRect rect) = 0;
    // 用纯色填充矩形
    virtual void FillRect(CRect rect, COLORREF color, BYTE alpha = 255) = 0;
    // 绘制矩形边框。如果dot_line为true，则为虚线。radius大于0时为圆角矩形
    virtual void DrawRectOutLine(CRect rect, COLORREF color, int width = 1, bool dot_line = false, BYTE alpha = 255, int radius = 0) = 0;
    // 使用当前画笔画线
    virtual void DrawLine(CPoint start_point, CPoint end_point, COLORREF color, BYTE alpha = 255) = 0;
    // 使用当前画笔画线，以start_point为起点，向上画长度为height的线段
    virtual void DrawLine(CPoint start_point, int height, COLORREF color, BYTE alpha = 255);
    virtual void SetTextColor(const COLORREF color, BYTE alpha = 255) = 0;
    // 绘制一个位图
    // （注意：当stretch_mode设置为StretchMode::FILL（填充）时，会设置绘图剪辑区域，如果之后需要绘制其他图形，
    // 需要重新设置绘图剪辑区域，否则图片外的区域会无法绘制）
    virtual void DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH, BYTE alpha = 255) = 0;
    virtual void DrawIcon(HICON hIcon, CPoint start_point, CSize size) = 0;
    virtual ~IDrawCommon() = default;

    //获取绘图上下文句柄。
    virtual CDC* GetDC() = 0;
    //获取文本宽度
    virtual int GetTextWidth(LPCTSTR lpszString) { return 0; }

    // 通过 IPluginDrawer 继承
    virtual void DrawWindowText(int x, int y, int w, int h, const wchar_t* lpszString, unsigned long color, Alignment align, bool multi_line, unsigned char alpha) override;
    virtual void SetDrawRect(int x, int y, int w, int h) override;
    virtual void FillRect(int x, int y, int w, int h, unsigned long color, unsigned char alpha) override;
    virtual void DrawRectOutLine(int x, int y, int w, int h, unsigned long color, int width, bool dot_line, unsigned char alpha, int radius) override;
    virtual void DrawBitmap(void* hbitmap, int x, int y, int w, int h, StretchMode stretch_mode, unsigned char alpha) override;
    virtual void DrawIcon(void* hIcon, int x, int y, int w, int h) override;
    virtual void DrawLine(int x1, int y1, int x2, int y2, unsigned long color, unsigned char alpha = 255) override;
};

namespace DrawCommonHelper
{
    /**
     * @brief 渲染器类型的枚举，新增渲染器类型时应当添加枚举到其中
     *
     */
    enum class RenderType
    {
        // 使用GDI
        DEFAULT,
        // 如果支持，使用D2D1
        D2D1,
        // 如果系统大于win8.1，则在使用D2D1时利用DirectComposition呈现结果
        D2D1_WITH_DCOMPOSITION
    };

    constexpr BYTE GDI_NO_MODIFIED_FLAG = 0x01;
    constexpr BYTE OPAQUE_ALPHA_VALUE = 0xFF;
    constexpr BYTE TRANSPARENT_ALPHA_VALUE = 0x00;
    constexpr BYTE AVAILABLE_MINIMUM_ALPHA = 0x02;
    constexpr BYTE GDI_MODIFIED_FLAG = 0x00;
}
