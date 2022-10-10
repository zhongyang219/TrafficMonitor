#pragma once
#include "CommonData.h"

class IDrawBuffer
{
public:
    virtual CDC* GetMemDC() = 0;

protected:
    ~IDrawBuffer() = default;
};

class IDrawCommon
{
public:
    // 拉伸模式
    enum class StretchMode
    {
        STRETCH, // 拉伸，会改变比例
        FILL,    // 填充，不改变比例，会裁剪长边
        FIT      // 适应，不会改变比例，不裁剪
    };

    virtual void SetBackColor(COLORREF back_color, BYTE alpha = 255) = 0;
    // 设置绘制文本的字体
    virtual void SetFont(CFont* pfont) = 0;
    // 在指定的矩形区域内绘制文本
    virtual void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align = Alignment::LEFT, bool draw_back_ground = false, bool multi_line = false, BYTE alpha = 255) = 0;
    // 设置绘图剪辑区域
    virtual void SetDrawRect(CRect rect) = 0;
    // 用纯色填充矩形
    virtual void FillRect(CRect rect, COLORREF color, BYTE alpha = 255) = 0;
    // 绘制矩形边框。如果dot_line为true，则为虚线
    virtual void DrawRectOutLine(CRect rect, COLORREF color, int width = 1, bool dot_line = false, BYTE alpha = 255) = 0;
    // 使用当前画笔画线
    virtual void DrawLine(CPoint start_point, int height, COLORREF color, BYTE alpha = 255) = 0;
    virtual void SetTextColor(const COLORREF color, BYTE alpha = 255) = 0;
    // 绘制一个位图
    // （注意：当stretch_mode设置为StretchMode::FILL（填充）时，会设置绘图剪辑区域，如果之后需要绘制其他图形，
    // 需要重新设置绘图剪辑区域，否则图片外的区域会无法绘制）
    virtual void DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH, BYTE alpha = 255) = 0;

protected:
    ~IDrawCommon() = default;
};

namespace DrawCommonHelper
{
    enum class RenderType
    {
        // 使用GDI
        Default,
        // 如果支持，使用D2D1
        D2D1
    };
    template <RenderType render_type>
    struct RenderTypeTag
    {
        constexpr static RenderType type = render_type;
    };
    using RenderTypeDefaultTag = RenderTypeTag<RenderType::Default>;
    using RenderTypeD2D1Tag = RenderTypeTag<RenderType::D2D1>;
}

template <class... Types>
struct variant_storage
{
    ~variant_storage() = delete;
};
template <class First, class... Other>
struct variant_storage<First, Other...>
{
    union
    {
        First m_head;
        variant_storage<Other...> m_tail;
    };
    ~variant_storage() = delete;
};

constexpr std::size_t STORAGE_INDEX = 0;
constexpr std::size_t TYPE_INDEX = 1;

template <class... T>
struct ez_tagged_union;
template <class TagEnum, class... T>
struct ez_tagged_union<TagEnum, T...>
{
    using type = std::tuple<std::aligned_storage_t<sizeof(::variant_storage<T...>), alignof(::variant_storage<T...>)>, TagEnum>;
};
template <class T>
struct ez_tagged_union<T>
{
    using type = void;
};
template <class TagEnum, class... T>
using EzTaggedUnion = typename ez_tagged_union<TagEnum, T...>::type;
