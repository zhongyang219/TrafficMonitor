#include "stdafx.h"
#include "D2D1Support.h"

#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "Dwrite.lib")

bool CD2D1Support::CheckSupport()
{
    static bool result = FunctionChecker::CheckFunctionExist(_T("D2d1.dll"), "D2D1CreateFactory");
    return result;
}

ID2D1Factory* CD2D1Support::GetFactory()
{
    static auto result = MakeStaticVariableWrapper<ID2D1Factory*>(
        [](ID2D1Factory** pp_factory)
        {
            *pp_factory = nullptr;
#ifdef DEBUG
            D2D1_FACTORY_OPTIONS d2d1_factory_options{D2D1_DEBUG_LEVEL_INFORMATION};
#else
            D2D1_FACTORY_OPTIONS d2d1_factory_options{D2D1_DEBUG_LEVEL_NONE};
#endif
            ThrowIfFailed(::D2D1CreateFactory(
                              D2D1_FACTORY_TYPE_SINGLE_THREADED,
                              d2d1_factory_options,
                              pp_factory),
                          "Create D2D1 factory failed.");
        },
        [](ID2D1Factory** pp_factory)
        {
            auto* p_factory = *pp_factory;
            RELEASE_COM(p_factory);
        });
    return result.Get();
}

bool DWriteSupport::CheckSupport()
{
    static bool result = FunctionChecker::CheckFunctionExist(_T("Dwrite.dll"), "DWriteCreateFactory");
    return result;
}

IDWriteFactory* DWriteSupport::GetFactory()
{
    static auto result = MakeStaticVariableWrapper<IDWriteFactory*>(
        [](IDWriteFactory** pp_factory)
        {
            *pp_factory = nullptr;
            ThrowIfFailed(::DWriteCreateFactory(
                              DWRITE_FACTORY_TYPE_SHARED,
                              __uuidof(IDWriteFactory),
                              reinterpret_cast<IUnknown**>(pp_factory)),
                          "Create DWrite factory failed.");
        },
        [](IDWriteFactory** pp_factory)
        {
            auto* p_factory = *pp_factory;
            RELEASE_COM(p_factory);
        });
    return result.Get();
}

CD2D1DCSupport::CD2D1DCSupport()
{
    //不支持D2D1或DWrite则直接返回
    if (!CheckSupport())
    {
        return;
    }

    D2D1_RENDER_TARGET_PROPERTIES d2d1_reder_target_properties{
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(
            DXGI_FORMAT_B8G8R8A8_UNORM,
            D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0,
        D2D1_RENDER_TARGET_USAGE_NONE,
        D2D1_FEATURE_LEVEL_DEFAULT};
    ThrowIfFailed<CD2D1Exception>(
        CD2D1Support::GetFactory()->CreateDCRenderTarget(&d2d1_reder_target_properties, &m_p_render_target),
        "Create d2d1 dc render target failed.");
    ThrowIfFailed<CD2D1Exception>(
        m_p_render_target->CreateSolidColorBrush(D2D1_COLOR_F{D2D1::ColorF::Black, 0.0f}, &m_p_soild_color_brush),
        "Create ID2D1SolidColorBrush failed.");
    ThrowIfFailed<CD2D1Exception>(
        m_p_render_target->CreateSolidColorBrush(D2D1_COLOR_F{D2D1::ColorF::Black, 0.0f}, &m_p_soild_back_color_brush),
        "Create ID2D1SolidColorBrush as back color failed.");
    {
        D2D1_STROKE_STYLE_PROPERTIES d2d1_stroke_style_properties{
            D2D1_CAP_STYLE_FLAT,
            D2D1_CAP_STYLE_FLAT,
            D2D1_CAP_STYLE_FLAT,
            D2D1_LINE_JOIN_MITER,
            10.0f,
            D2D1_DASH_STYLE_DASH,
            1.0f};
        ThrowIfFailed<CD2D1Exception>(
            CD2D1Support::GetFactory()->CreateStrokeStyle(d2d1_stroke_style_properties, NULL, 0, &m_p_ps_dot_style),
            "Create GDI PS_DOT like stroke style failed.");
    }
    ThrowIfFailed<CD2D1Exception>(
        DWriteSupport::GetFactory()->GetGdiInterop(&m_p_dwrite_gdi_interop),
        "Create DWrite GDI interop interface failed.");
};

CD2D1DCSupport::~CD2D1DCSupport()
{
    RELEASE_COM(m_p_soild_color_brush);
    RELEASE_COM(m_p_soild_back_color_brush);
    RELEASE_COM(m_p_ps_dot_style);
    RELEASE_COM(m_p_dwrite_gdi_interop);
}

bool CD2D1DCSupport::CheckSupport()
{
    return CD2D1Support::CheckSupport() && DWriteSupport::CheckSupport();
}

auto CD2D1DCSupport::GetRenderTarget()
    -> Microsoft::WRL::ComPtr<ID2D1DCRenderTarget>
{
    return m_p_render_target;
}

auto CD2D1DCSupport::GetWeakRenderTarget()
    -> ID2D1DCRenderTarget*
{
    return m_p_render_target.Get();
}

auto CD2D1DCSupport::GetWeakSolidColorBrush()
    -> ID2D1SolidColorBrush*
{
    return m_p_soild_color_brush;
}

auto CD2D1DCSupport::GetWeakPsDotStyle()
    -> ID2D1StrokeStyle*
{
    return m_p_ps_dot_style;
}

auto CD2D1DCSupport::GetWeakDWriteGdiInterop()
    -> IDWriteGdiInterop*
{
    return m_p_dwrite_gdi_interop;
}

auto CD2D1DCSupport::GetWeakSoildBackColorBrush()
    -> ID2D1SolidColorBrush*
{
    return m_p_soild_back_color_brush;
}