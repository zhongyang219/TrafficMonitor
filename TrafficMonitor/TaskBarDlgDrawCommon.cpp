#include "stdafx.h"
#include <unordered_map>
#include "TaskBarDlgDrawCommon.h"
#include "D2D1Support.h"
#include "TrafficMonitor.h"
#include "DrawCommon.h"
#include "WIC.h"

#undef min

using Microsoft::WRL::ComPtr;

// 其实可以把Verify方法作为构造函数
template <class T, T FAILED_VALUE>
struct ReturnValueVerifier
{
    static bool Verify(T value) noexcept
    {
        if (value != FAILED_VALUE)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    template <class Cleaner>
    static bool Verify(T value, Cleaner&& cleaner) noexcept(noexcept(cleaner))
    {
        if (value != FAILED_VALUE)
        {
            return true;
        }
        else
        {
            cleaner();
            return false;
        }
    }
    template <class Cleaner, class Func>
    static bool Verify(T value, Cleaner&& cleaner, Func&& do_before_clean)
    {
        if (value != FAILED_VALUE)
        {
            return true;
        }
        else
        {
            do_before_clean();
            cleaner();
            return false;
        }
    }
};
using Win32BOOLVerifier = ReturnValueVerifier<BOOL, 0>;
using Win32IntVerifier = ReturnValueVerifier<int, 0>;
using Win32COLORREFVerifier = ReturnValueVerifier<COLORREF, CLR_INVALID>;
using Win32HBITMAPVerifier = ReturnValueVerifier<HBITMAP, nullptr>;
using Win32HDCVerifier = ReturnValueVerifier<HDC, nullptr>;
using Win32HFONTVerifier = ReturnValueVerifier<HFONT, nullptr>;
using Win32HGDIOBJVerifier = ReturnValueVerifier<HGDIOBJ, nullptr>;

void LogWin32ApiErrorMessage() noexcept
{
    auto error_code = ::GetLastError();
    // 写入系统格式化后的错误信息
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
}

void DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler(CHResultException& ex)
{
    // 禁用D2D绘图
    theApp.m_taskbar_data.disable_d2d = true;
    LogHResultException(ex);
    ::MessageBox(NULL, CCommon::LoadText(IDS_D2DDRAWCOMMON_ERROR_TIP), NULL, MB_OK | MB_ICONWARNING);
}

HDC DrawCommonHelper::Get1x1AlphaEqual1DC()
{
    const static auto result = MakeStaticVariableWrapper<std::tuple<HDC, HGDIOBJ>>(
        [](auto* p_content)
        {
            auto& ref_dc = std::get<0>(*p_content);
            auto& ref_old_hbitmap = std::get<1>(*p_content);

            ref_dc = ::CreateCompatibleDC(NULL);
            auto bitmap_info = TaskBarDlgUser32DrawTextHook::Details::GetArgb32BitmapInfo(1, 1);
            void* p_data{};
            auto current_hbitmap = ::CreateDIBSection(
                ref_dc,
                &bitmap_info,
                DIB_RGB_COLORS,
                &p_data,
                NULL,
                0);
            if (!Win32HBITMAPVerifier::Verify(current_hbitmap))
            {
                throw std::runtime_error{"Create ARGB32 DIB (size = 1x1, alpha = 1) failed."};
            }
            ref_old_hbitmap = ::SelectObject(ref_dc, current_hbitmap);
            ::GdiFlush();
            auto p_pixel_data = reinterpret_cast<BYTE*>(p_data);
            ::memset(p_data, 0, 3);
            p_pixel_data[3] = DrawCommonHelper::GDI_NO_MODIFIED_FLAG;
        },
        [](auto* p_content)
        {
            auto& ref_dc = std::get<0>(*p_content);
            auto& ref_old_hbitmap = std::get<1>(*p_content);

            auto previously_used_hbitmap = ::SelectObject(ref_dc, ref_old_hbitmap);
            ::DeleteObject(previously_used_hbitmap);
            ::DeleteDC(ref_dc);
        });
    return std::get<0>(result.Get());
}

CTaskBarDlgDrawCommonSupport::CTaskBarDlgDrawCommonSupport()
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
        CD2D1Support::GetFactory()->CreateStrokeStyle(
            d2d1_stroke_style_properties,
            NULL,
            0,
            &m_p_ps_dot_like_style),
        "Create GDI PS_DOT like stroke style failed.");

    UINT flag = D3D10_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef DEBUG
    flag |= D3D10_CREATE_DEVICE_DEBUG;
#endif
    m_device1.SetFlags(flag);
    RecreateDevice();
}

auto CTaskBarDlgDrawCommonSupport::GetDevice() noexcept
    -> CD3D10Device1&
{
    return m_device1;
}

auto CTaskBarDlgDrawCommonSupport::GetPsDotLikeStyle() noexcept
    -> Microsoft::WRL::ComPtr<ID2D1StrokeStyle>
{
    return m_p_ps_dot_like_style;
}

void CTaskBarDlgDrawCommonSupport::RecreateDevice()
{
    auto default_adapter1 =
        CD3D10Support1::GetDeviceList(true).front();
    m_device1.Recreate(default_adapter1);
}

void CTaskBarDlgDrawCommonWindowSupport::GpuHelper::RecreateResource(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1, const D2D1_SIZE_U size)
{
    const static D2D1_PIXEL_FORMAT format =
        D2D1::PixelFormat(
            PIXEL_FORMAT,
            D2D1_ALPHA_MODE_PREMULTIPLIED);
    const static D2D1_RENDER_TARGET_PROPERTIES properties =
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            format,
            DEFAULT_DPI,
            DEFAULT_DPI,
            D2D1_RENDER_TARGET_USAGE_NONE);
    constexpr static auto TIP_WHEN_CREATE_ID2D1RENDERTARGET_FAILED =
        "Create d2d1 render target from dxgi surface failed.";

    D3D10_TEXTURE2D_DESC description = {};
    description.ArraySize = 1;
    description.Format = PIXEL_FORMAT;
    description.Width = size.width;
    description.Height = size.height;
    description.MipLevels = 1;
    description.SampleDesc.Count = 1;
    description.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
    description.CPUAccessFlags = 0;
    description.Usage = D3D10_USAGE_DEFAULT;
    ThrowIfFailed<CD3D10Exception1>(
        p_device1->CreateTexture2D(
            &description,
            NULL,
            &m_p_gdi_final_texture),
        "Create m_p_gdi_final_texture(type = ID3D10Texture2D) failed.");
    ThrowIfFailed<CD3D10Exception1>(
        m_p_gdi_final_texture->QueryInterface(IID_PPV_ARGS(&m_p_gdi_finial_surface)),
        "Get IDXGISurface1 from GDI interop result texture failed.");

    auto p_render_target_texture = CreateGdiCompatibleTexture(p_device1, size);
    ThrowIfFailed<CD3D10Exception1>(
        p_render_target_texture->QueryInterface(IID_PPV_ARGS(&m_p_render_target_surface)),
        "Get IDXGISurface1 from D2D render target texture failed.");
    m_alpha_value_effect
        .SetOutputSize(size.width, size.height)
        .SetPsByteCode(GetPsAlphaValueReducer());

    ThrowIfFailed<CD2D1Exception>(
        CD2D1Support::GetFactory()->CreateDxgiSurfaceRenderTarget(
            m_p_render_target_surface.Get(),
            &properties,
            &m_p_render_target),
        TIP_WHEN_CREATE_ID2D1RENDERTARGET_FAILED);
    auto sp_cache = m_wp_bitmap_cache.lock();
    if (sp_cache)
    {
        sp_cache->RebindRenderTarget(m_p_render_target);
    }

    ThrowIfFailed<CD2D1Exception>(
        m_p_render_target->CreateSolidColorBrush(
            m_foreground_color,
            &m_p_foreground_color_brush),
        "Call function CreateSolidColorBrush to cerate foreground solid color brush failed.");
    ThrowIfFailed<CD2D1Exception>(
        m_p_render_target->CreateSolidColorBrush(
            m_background_color,
            &m_p_background_color_brush),
        "Call function CreateSolidColorBrush to cerate background solid color brush failed.");
}

void CTaskBarDlgDrawCommonWindowSupport::GpuHelper::SetSolidBrushColor(ComPtr<ID2D1SolidColorBrush> p_brush, D2D1_COLOR_F d2d1_color_f) noexcept
{
    p_brush->SetColor(d2d1_color_f);
}

CTaskBarDlgDrawCommonWindowSupport::GpuHelper::GpuHelper(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1)
    : m_alpha_value_effect{p_device1}
{
}

auto CTaskBarDlgDrawCommonWindowSupport::GpuHelper::CreateGdiCompatibleTexture(ComPtr<ID3D10Device1> p_device1, const D2D1_SIZE_U size)
    -> Microsoft::WRL::ComPtr<ID3D10Texture2D>
{
    D3D10_TEXTURE2D_DESC description = {};
    description.ArraySize = 1;
    description.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
    description.Format = PIXEL_FORMAT;
    description.Width = size.width;
    description.Height = size.height;
    description.MipLevels = 1;
    description.SampleDesc.Count = 1;
    description.MiscFlags = D3D10_RESOURCE_MISC_GDI_COMPATIBLE;

    Microsoft::WRL::ComPtr<ID3D10Texture2D> p_texture2d{};

    ThrowIfFailed<CD3D10Exception1>(
        p_device1->CreateTexture2D(
            &description,
            NULL,
            &p_texture2d),
        "CTaskBarDlgDrawCommonWindowBuffer create texture2D failed.");

    return p_texture2d;
}

auto CTaskBarDlgDrawCommonWindowSupport::GpuHelper::CreateGdiInteropTexture(ComPtr<ID3D10Device1> p_device1, const D2D1_SIZE_U size)
    -> Microsoft::WRL::ComPtr<ID3D10Texture2D>
{
    D3D10_TEXTURE2D_DESC description = {};
    description.ArraySize = 1;
    description.BindFlags = D3D10_BIND_RENDER_TARGET;
    description.Format = PIXEL_FORMAT;
    description.Width = size.width;
    description.Height = size.height;
    description.MipLevels = 1;
    description.SampleDesc.Count = 1;
    description.MiscFlags =
        D3D10_RESOURCE_MISC_GDI_COMPATIBLE | D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX;

    Microsoft::WRL::ComPtr<ID3D10Texture2D> p_render_target_texture{};

    ThrowIfFailed<CD3D10Exception1>(
        p_device1->CreateTexture2D(
            &description,
            NULL,
            &p_render_target_texture),
        "CTaskBarDlgDrawCommonWindowBuffer create texture2D failed.");

    return p_render_target_texture;
}

void CTaskBarDlgDrawCommonWindowSupport::GpuHelper::Resize(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1, const D2D1_SIZE_U size)
{
    if (m_render_target_size.width != size.width || m_render_target_size.height != size.height)
    {
        RecreateResource(p_device1, size);
        m_render_target_size = size;
    }
}

auto CTaskBarDlgDrawCommonWindowSupport::GpuHelper::GetPsAlphaValueReducer() noexcept
    -> Microsoft::WRL::ComPtr<ID3D10Blob>
{
    const static auto result = MakeStaticVariableWrapper<CShader>(
        [](CShader* p_content)
        {
            p_content->SetCode(
                         CIMAGE2DEFFECT_SHADER_VS_OUTPUT_DECLARATION
                         R"(
SamplerState input_sampler : register(s0);
Texture2D input_texture : register(t0);

float4 PS(VsOutput ps_in) : SV_TARGET
{
    float4 color = input_texture.Sample(input_sampler, ps_in.texcoord);
    [flatten] if (color.w > )" TRAFFICMONITOR_ONE_IN_255_FLOOR " && color.w < " TRAFFICMONITOR_ONE_IN_255_CEIL ")"
                         R"(
    {
        color.w = 0.0;
        return color;
    }
    [flatten] if (color.w == 0)
    {
        color.w = 1.0;
        return color;
    }
    return color;
}
)")
                .SetEntryPoint("PS")
                .SetName("PsGdiTexturePostprocessor")
                .SetTarget("ps_4_1")
                .SetFlags1(D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS);
        });
    return result.Get().Compile();
}

CTaskBarDlgDrawCommonWindowSupport::CTaskBarDlgDrawCommonWindowSupport(CTaskBarDlgDrawCommonSupport& ref_taskbdlg_draw_common_support)
    : CDX10DeviceResource1{ref_taskbdlg_draw_common_support.GetDevice()},
      m_p_device1{ref_taskbdlg_draw_common_support.GetDevice().Get()},
      m_ref_d3d10_device1{ref_taskbdlg_draw_common_support.GetDevice()},
      m_gpu_helper{m_p_device1}, m_ref_taskbdlg_draw_common_support{ref_taskbdlg_draw_common_support}
{
}

void CTaskBarDlgDrawCommonWindowSupport::OnDeviceRecreate(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1) noexcept
{
    m_need_recreate = true;
}

void CTaskBarDlgDrawCommonWindowSupport::Resize(const D2D1_SIZE_U size)
{
    if (m_need_recreate)
    {
        auto p_gpu_helper = std::addressof(m_gpu_helper);
        Destroy(p_gpu_helper);
        EmplaceAt(p_gpu_helper, m_p_device1);
        m_need_recreate = false;
    }
    m_gpu_helper.Resize(m_p_device1, size);
}

auto CTaskBarDlgDrawCommonWindowSupport::GetRenderTarget() noexcept
    -> Microsoft::WRL::ComPtr<ID2D1RenderTarget>
{
    return m_gpu_helper.m_p_render_target;
}

void CTaskBarDlgDrawCommonWindowSupport::InitGdiInteropTexture(void* p_data, std::size_t data_size)
{
    D3D10_TEXTURE2D_DESC description = {};
    description.ArraySize = 1;
    description.BindFlags = D3D10_BIND_SHADER_RESOURCE;
    description.Format = PIXEL_FORMAT;
    description.Width = m_gpu_helper.m_render_target_size.width;
    description.Height = m_gpu_helper.m_render_target_size.height;
    description.MipLevels = 1;
    description.SampleDesc.Count = 1;
    description.Usage = D3D10_USAGE_DEFAULT;
    D3D10_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = p_data;
    subresource_data.SysMemPitch = m_gpu_helper.m_render_target_size.width * 4;
    ThrowIfFailed<CD3D10Exception1>(
        m_p_device1->CreateTexture2D(
            &description,
            &subresource_data,
            &m_gpu_helper.m_p_gdi_initial_texture),
        "Create m_p_gdi_initial_texture(type = ID3D10Texture2D) failed.");
    ThrowIfFailed<CD3D10Exception1>(
        m_gpu_helper.m_p_gdi_initial_texture->QueryInterface(
            IID_PPV_ARGS(&m_gpu_helper.m_p_gdi_initial_surface)),
        "Get IDXGISurface1 from GDI interop initial texture failed.");
}

void CTaskBarDlgDrawCommonWindowSupport::SetFont(HFONT h_font)
{
    m_dwrite_helper.SetFont(h_font);
}

void CTaskBarDlgDrawCommonWindowSupport::SetBackColor(COLORREF color, BYTE alpha) noexcept
{
    auto d2d1_color = CRenderTarget::COLORREF_TO_D2DCOLOR(color, alpha);
    if (!IsBitwiseEquality(d2d1_color, m_gpu_helper.m_background_color))
    {
        GpuHelper::SetSolidBrushColor(m_gpu_helper.m_p_background_color_brush, d2d1_color);
        m_gpu_helper.m_background_color = d2d1_color;
    }
}

auto CTaskBarDlgDrawCommonWindowSupport::SetForeColor(COLORREF color, BYTE alpha) noexcept
    -> CTaskBarDlgDrawCommonWindowSupport*
{
    auto d2d1_color = CRenderTarget::COLORREF_TO_D2DCOLOR(color, alpha);
    if (!IsBitwiseEquality(d2d1_color, m_gpu_helper.m_background_color))
    {
        GpuHelper::SetSolidBrushColor(m_gpu_helper.m_p_foreground_color_brush, d2d1_color);
        m_gpu_helper.m_foreground_color = d2d1_color;
    }
    return this;
}

auto CTaskBarDlgDrawCommonWindowSupport::GetTextFormat() noexcept
    -> Microsoft::WRL::ComPtr<IDWriteTextFormat>
{
    return m_dwrite_helper.GetTextFormat();
}

auto CTaskBarDlgDrawCommonWindowSupport::GetRawPsDotLikeStyle() noexcept -> ID2D1StrokeStyle*
{
    return m_ref_taskbdlg_draw_common_support.GetPsDotLikeStyle().Get();
}

auto CTaskBarDlgDrawCommonWindowSupport::GetGdiInitalTextureSurface() noexcept
    -> Microsoft::WRL::ComPtr<IDXGISurface1>
{
    return m_gpu_helper.m_p_gdi_initial_surface;
}

auto CTaskBarDlgDrawCommonWindowSupport::GetGdiFinalTextureSurface() noexcept
    -> Microsoft::WRL::ComPtr<IDXGISurface1>
{
    return m_gpu_helper.m_p_gdi_finial_surface;
}

auto CTaskBarDlgDrawCommonWindowSupport::GetSize() const noexcept
    -> D2D1_SIZE_U
{
    return m_gpu_helper.m_render_target_size;
}

auto CTaskBarDlgDrawCommonWindowSupport::GetFont() noexcept
    -> HFONT
{
    return m_dwrite_helper.GetFont();
}

#define TRAFFICMONITOR_ENUM_AND_EXPLANATION(enumeration, explanation) \
    enumeration, "Handling device loss. HR = " #enumeration ": " explanation

void CTaskBarDlgDrawCommonWindowSupport::RecreateDevice()
{
    const static std::unordered_map<HRESULT, const char*> removed_reason{
        {TRAFFICMONITOR_ENUM_AND_EXPLANATION(
            DXGI_ERROR_DEVICE_HUNG,
            "The graphics driver stopped responding because of an invalid combination of graphics commands sent by the app. If you get this error repeatedly, it is a likely indication that your app caused the device to hang and needs to be debugged.")},
        {TRAFFICMONITOR_ENUM_AND_EXPLANATION(
            DXGI_ERROR_DEVICE_REMOVED,
            "The graphics device has been physically removed, turned off, or a driver upgrade has occurred. ")},
        {TRAFFICMONITOR_ENUM_AND_EXPLANATION(
            DXGI_ERROR_DEVICE_RESET,
            "The graphics device failed because of a badly formed command. If you get this error repeatedly, it may mean that your code is sending invalid drawing commands.")},
        {TRAFFICMONITOR_ENUM_AND_EXPLANATION(
            DXGI_ERROR_DRIVER_INTERNAL_ERROR,
            "The graphics driver encountered an error and reset the device.")},
        {TRAFFICMONITOR_ENUM_AND_EXPLANATION(
            DXGI_ERROR_INVALID_CALL,
            "The application provided invalid parameter data. If you get this error even once, it means that your code caused the device removed condition and must be debugged.")},
        {TRAFFICMONITOR_ENUM_AND_EXPLANATION(
            S_OK,
            "Returned when a graphics device was enabled, disabled, or reset without invalidating the current graphics device. For example, this error code can be returned if an app is using WARP and a hardware adapter becomes available.")}};

    auto hr = m_p_device1->GetDeviceRemovedReason();
    auto it = removed_reason.find(hr);
    if (it != removed_reason.end())
    {
        CCommon::WriteLog(it->second, theApp.m_log_path.c_str());
    }
    else
    {
        constexpr static auto unknown_reason{"Recreate the device for an unknown reason."};
        CCommon::WriteLog(unknown_reason, theApp.m_log_path.c_str());
    }

    m_ref_taskbdlg_draw_common_support.RecreateDevice();
}

auto CTaskBarDlgDrawCommonWindowSupport::GetRenderTargetSurface() noexcept
    -> Microsoft::WRL::ComPtr<IDXGISurface1>
{
    return m_gpu_helper.m_p_render_target_surface;
}

auto CTaskBarDlgDrawCommonWindowSupport::DrawAlphaValueReduceEffect() noexcept
    -> CD3D10DrawCallWaiter
{
    constexpr std::array<FLOAT, 4> transparent_black{.0f, .0f, .0f, .0f};

    m_gpu_helper.m_alpha_value_effect
        .SetOutputTexture(m_gpu_helper.m_p_gdi_final_texture)
        .ClearOnly(transparent_black);
    return m_gpu_helper.m_alpha_value_effect
        .SetInputTexture(m_gpu_helper.m_p_gdi_initial_texture)
        .ApplyPipelineConfig()
        .Draw();
}

auto CTaskBarDlgDrawCommonWindowSupport::GetRawForeSolidColorBruch() noexcept
    -> ID2D1SolidColorBrush*
{
    return m_gpu_helper.m_p_foreground_color_brush.Get();
}

auto CTaskBarDlgDrawCommonWindowSupport::GetRawBackSolidColorBruch() noexcept
    -> ID2D1SolidColorBrush*
{
    return m_gpu_helper.m_p_background_color_brush.Get();
}

void CTaskBarDlgDrawCommonWindowSupport::RebindD2D1BitmapCache(std::weak_ptr<CD2D1BitmapCache> wp_cache)
{
    m_gpu_helper.m_wp_bitmap_cache = wp_cache;
}

auto CTaskBarDlgDrawCommonWindowSupport::GetCachedBitmap(HBITMAP hbitmap) const
    -> Microsoft::WRL::ComPtr<ID2D1Bitmap>
{
    auto sp_cache = m_gpu_helper.m_wp_bitmap_cache.lock();
    if (sp_cache)
    {
        auto result = sp_cache->GetCachedBitmap(hbitmap);
        return result;
    }
    return {};
}

void CTaskBarDlgDrawCommonWindowSupport::DWriteHelper::SetFont(HFONT h_font)
{
    if (m_h_last_font != h_font)
    {
        ::GetObject(h_font, sizeof(m_last_font), &m_last_font);

        auto abs_font_height_f =
            static_cast<float>(std::abs(m_last_font.lfHeight));
        auto dwrite_font_weight =
            static_cast<DWRITE_FONT_WEIGHT>(m_last_font.lfWeight);
        auto dwrite_font_style =
            m_last_font.lfItalic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL;
        ThrowIfFailed<CDWriteException>(
            DWriteSupport::GetFactory()->CreateTextFormat(
                theApp.m_taskbar_data.font.name,
                NULL,
                dwrite_font_weight,
                dwrite_font_style,
                DWRITE_FONT_STRETCH_NORMAL,
                abs_font_height_f,
                L"",
                &m_p_text_format),
            "Create D2D1 Text Format failed.");

        m_h_last_font = h_font;
    }
}

auto CTaskBarDlgDrawCommonWindowSupport::DWriteHelper::GetFont() noexcept
    -> HFONT
{
    return m_h_last_font;
}

auto CTaskBarDlgDrawCommonWindowSupport::DWriteHelper::GetTextFormat() noexcept
    -> ComPtr<IDWriteTextFormat>
{
    return m_p_text_format;
}

namespace TaskBarDlgUser32DrawTextHook
{
    namespace Details
    {
        auto GetArgb32BitmapInfo(CRect rect) noexcept
            -> ::BITMAPINFO
        {
            LONG width = std::abs(rect.Width());
            LONG height = std::abs(rect.Height());
            return GetArgb32BitmapInfo(width, height);
        }

        auto GetArgb32BitmapInfo(LONG width, LONG height) noexcept
            -> ::BITMAPINFO
        {
            ::BITMAPINFO result;
            memset(&result, 0, sizeof(BITMAPINFO));
            result.bmiHeader.biSize = sizeof(result.bmiHeader);
            // 保证是自上而下
            result.bmiHeader.biWidth = static_cast<LONG>(width);
            result.bmiHeader.biHeight = -static_cast<LONG>(height);
            result.bmiHeader.biPlanes = 1;
            result.bmiHeader.biBitCount = 32;
            result.bmiHeader.biCompression = BI_RGB;
            return result;
        }

        template <class DrawTextFunctionNamespace, class... DrawTextArgs>
        int ReplacedDrawTextCommon(const CDrawTextReplacedFunction<DrawTextFunctionNamespace>& ref_this, DrawTextArgs&&... draw_text_args)
        {
            constexpr std::size_t hdc_index = 0;
            constexpr std::size_t lpchText_index = 1;
            constexpr std::size_t cchText_index = 2;
            constexpr std::size_t lprc_index = 3;
            constexpr std::size_t format_index = 4;

            auto p_original_function = DrawTextFunctionNamespace::GetOriginalFunction();
            auto args_tuple = std::forward_as_tuple(draw_text_args...);
            auto input_hdc = std::get<hdc_index>(args_tuple);
            auto input_format = std::get<format_index>(args_tuple);
            bool is_only_calculate_size = input_format & DT_CALCRECT;
            if (ref_this.m_state.m_on_draw_text_call_matched_hdc != input_hdc || is_only_calculate_size)
            {
                return (*p_original_function)(std::forward<DrawTextArgs>(draw_text_args)...);
            }
            auto text_length = std::get<cchText_index>(args_tuple);
            if (text_length == 0)
            {
                return User32DrawTextManager::CUSTOM_SUCCESS;
            }
            // https://stackoverflow.com/questions/42221322/how-to-draw-text-with-transparency-using-gdi
            if (p_original_function)
            {
                HDC text_dc{NULL};
                text_dc = ::CreateCompatibleDC(NULL);
                if (!Win32HDCVerifier::Verify(text_dc))
                {
                    return 0;
                }
                auto clean_after_creating_dc = [&text_dc]() noexcept
                {
                    ::DeleteDC(text_dc);
                };

                auto& ref_input_rect = std::get<lprc_index>(args_tuple);
                auto bitmap_info = GetArgb32BitmapInfo(ref_input_rect);
                void* p_data;
                auto text_hbitmap = ::CreateDIBSection(
                    text_dc,
                    &bitmap_info,
                    DIB_RGB_COLORS,
                    &p_data,
                    NULL,
                    0);
                if (!Win32HBITMAPVerifier::Verify(text_hbitmap))
                {
                    clean_after_creating_dc();
                    return 0;
                }
                auto clean_after_creating_text_hbitmap =
                    [&text_hbitmap, &clean_after_creating_dc]() noexcept
                {
                    ::DeleteObject(text_hbitmap);
                    clean_after_creating_dc();
                };

                HGDIOBJ old_hbitmap = ::SelectObject(text_dc, text_hbitmap);
                auto clean_after_selecting_text_hbitmap =
                    [&text_dc, &old_hbitmap, &clean_after_creating_text_hbitmap]() noexcept
                {
                    ::SelectObject(text_dc, old_hbitmap);
                    clean_after_creating_text_hbitmap();
                };

                auto hfont = ::GetCurrentObject(input_hdc, OBJ_FONT);
                if (!Win32HGDIOBJVerifier::Verify(
                        hfont,
                        clean_after_selecting_text_hbitmap))
                {
                    return 0;
                }
                HGDIOBJ old_hfont = ::SelectObject(text_dc, hfont);
                auto clean_after_selecting_text_hfont =
                    [&text_dc, &old_hfont, &clean_after_selecting_text_hbitmap]() noexcept
                {
                    ::SelectObject(text_dc, old_hfont);
                    clean_after_selecting_text_hbitmap();
                };

                if (!Win32COLORREFVerifier::Verify(
                        ::SetTextColor(text_dc, 0x00FFFFFF),
                        clean_after_selecting_text_hfont))
                {
                    return 0;
                }
                if (!Win32COLORREFVerifier::Verify(
                        ::SetBkColor(text_dc, 0x00000000),
                        clean_after_selecting_text_hfont))
                {
                    return 0;
                }
                if (!Win32IntVerifier::Verify(
                        ::SetBkMode(text_dc, OPAQUE),
                        clean_after_selecting_text_hfont))
                {
                    return 0;
                }
                CRect modified_rect = ref_input_rect;
                modified_rect.MoveToXY(0, 0);
                auto&& replaced_dc_and_text_rect = std::make_tuple(
                    text_dc,
                    std::get<lpchText_index>(args_tuple),
                    std::get<cchText_index>(args_tuple),
                    modified_rect);
                if (!Win32IntVerifier::Verify(
                        ReplaceNthConsecutiveArgumentAndApply<hdc_index>(
                            std::move(replaced_dc_and_text_rect), p_original_function,
                            std::forward<DrawTextArgs>(draw_text_args)...),
                        clean_after_selecting_text_hfont))
                {
                    return 0;
                }
                std::size_t width = modified_rect.right;
                std::size_t height = modified_rect.bottom;
                auto p_pixel_data = reinterpret_cast<BYTE*>(p_data);
                auto color = ::GetTextColor(input_hdc);
                if (!Win32COLORREFVerifier::Verify(
                        color,
                        clean_after_selecting_text_hfont))
                {
                    return 0;
                }
                BYTE r = GetRValue(color);
                BYTE g = GetGValue(color);
                BYTE b = GetBValue(color);
                if (!Win32BOOLVerifier::Verify(
                        ::GdiFlush(),
                        clean_after_selecting_text_hfont))
                {
                    return 0;
                }
                // 切换显示器时可能引发越界访问（未证实）
                // __try
                for (std::size_t y = 0; y < height; ++y)
                {
                    for (std::size_t x = 0; x < width; ++x)
                    {
                        std::uint16_t rgb_sum;
                        rgb_sum = p_pixel_data[0] + p_pixel_data[1] + p_pixel_data[2];
                        if (rgb_sum == 0)
                        {
                            std::advance(p_pixel_data, 4);
                        }
                        else
                        {
                            *p_pixel_data++ = b;
                            *p_pixel_data++ = g;
                            *p_pixel_data++ = r;
                            auto a = static_cast<BYTE>(rgb_sum * 0.334f);
                            *p_pixel_data++ = a;
                        }
                    }
                }
                // __except(EXCEPTION_EXECUTE_HANDLER)
                LPRECT p_input_rect = std::get<lprc_index>(args_tuple);
                ::BLENDFUNCTION blend_function{
                    AC_SRC_OVER,
                    0,
                    0xFF,
                    AC_SRC_ALPHA};
                auto width_i32 = static_cast<std::int32_t>(width);
                auto height_i32 = static_cast<std::int32_t>(height);
                if (!Win32IntVerifier::Verify(
                        ::AlphaBlend(
                            input_hdc,
                            p_input_rect->left, p_input_rect->top,
                            width_i32, height_i32,
                            text_dc,
                            0, 0,
                            width_i32, height_i32,
                            blend_function),
                        clean_after_selecting_text_hfont))
                {
                    return 0;
                }

                clean_after_selecting_text_hfont();
                return User32DrawTextManager::CUSTOM_SUCCESS;
            }
            return 0;
        }

        const CDrawTextReplacedFunction<User32DrawTextManager::A> on_draw_text_a_call{
            [p_this = &on_draw_text_a_call](HDC hdc, LPCSTR lpchText, int cchText, LPRECT lprc, UINT format)
            { return ReplacedDrawTextCommon(*p_this, hdc, lpchText, cchText, lprc, format); }};
        const CDrawTextReplacedFunction<User32DrawTextManager::W> on_draw_text_w_call{
            [p_this = &on_draw_text_w_call](HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
            { return ReplacedDrawTextCommon(*p_this, hdc, lpchText, cchText, lprc, format); }};
        const CDrawTextReplacedFunction<User32DrawTextManager::ExA> on_draw_text_exa_call{
            [p_this = &on_draw_text_exa_call](HDC hdc, LPSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp)
            { return ReplacedDrawTextCommon(*p_this, hdc, lpchText, cchText, lprc, format, lpdtp); }};
        const CDrawTextReplacedFunction<User32DrawTextManager::ExW> on_draw_text_exw_call{
            [p_this = &on_draw_text_exw_call](HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp)
            { return ReplacedDrawTextCommon(*p_this, hdc, lpchText, cchText, lprc, format, lpdtp); }};
    }

    EnableAllReplaceFunctionGuard::~EnableAllReplaceFunctionGuard()
    {
        Details::on_draw_text_a_call.Disable();
        Details::on_draw_text_w_call.Disable();
        Details::on_draw_text_exa_call.Disable();
        Details::on_draw_text_exw_call.Disable();

        Details::on_draw_text_a_call.Reset();
        Details::on_draw_text_w_call.Reset();
        Details::on_draw_text_exa_call.Reset();
        Details::on_draw_text_exw_call.Reset();
    }

    auto EnableAllReplaceFunction(Details::DrawTextReplacedFunctionState& ref_state) noexcept
        -> class EnableAllReplaceFunctionGuard
    {
        Details::on_draw_text_a_call.Enable(ref_state);
        Details::on_draw_text_w_call.Enable(ref_state);
        Details::on_draw_text_exa_call.Enable(ref_state);
        Details::on_draw_text_exw_call.Enable(ref_state);

        return {};
    }
}

void CD2D1BitmapCache::Cache::Update(Microsoft::WRL::ComPtr<ID2D1RenderTarget> p_render_target, HBITMAP hbitmap)
{
    auto p_updated_cache = m_cache_initializer(p_render_target, hbitmap);
    m_cache = p_updated_cache;
    m_init_timestamp = std::chrono::steady_clock::now();
}

bool CD2D1BitmapCache::HeapData::IsCacheExpire(const Cache& cache, std::chrono::steady_clock::time_point now)
{
    auto survival_time = std::chrono::duration_cast<std::chrono::seconds>(
        now - cache.m_init_timestamp);
    if (survival_time > m_cache_expire_interval)
    {
        return true;
    }
    return false;
}

CD2D1BitmapCache::CD2D1BitmapCache(ComPtr<ID2D1RenderTarget> p_render_target)
    : m_p_render_target{p_render_target}
{
}

CD2D1BitmapCache::~CD2D1BitmapCache()
{
    m_gc_thread.detach();
}

void CD2D1BitmapCache::RebindRenderTarget(ComPtr<ID2D1RenderTarget> p_render_target)
{
    if (p_render_target == m_p_render_target)
    {
        return;
    }
    m_p_render_target = p_render_target;
    RecreateAllHBitmaps();
}

void CD2D1BitmapCache::AddHBitmap(HBITMAP hbitmap, CacheInitializer initializer)
{
    TRAFFICMONITOR_CD2D1BITMAPCACHE_LOCK_CACHE_MAP_AND_EXPIRE_INTERVAL(m_sp_data);
    if (IsHBitmapExist(hbitmap))
    {
        return;
    }
    try
    {
        auto p_d2d1_bitmap = initializer(m_p_render_target, hbitmap);
        m_sp_data->m_cache_map[hbitmap] = {
            p_d2d1_bitmap,
            std::chrono::steady_clock::now(),
            initializer};
    }
    catch (CWICException& ex)
    {
        LogHResultException(ex);
    }
}

void CD2D1BitmapCache::AddHBitmap(HBITMAP hbitmap)
{
    AddHBitmap(hbitmap, CD2D1BitmapCache::CreateD2D1BitmapFromHBitmap);
}

void CD2D1BitmapCache::RemoveHBitmap(HBITMAP hbitmap)
{
    TRAFFICMONITOR_CD2D1BITMAPCACHE_LOCK_CACHE_MAP_AND_EXPIRE_INTERVAL(m_sp_data);
    auto it = m_sp_data->m_cache_map.find(hbitmap);
    if (it != m_sp_data->m_cache_map.end())
    {
        m_sp_data->m_cache_map.erase(it);
    }
}

void CD2D1BitmapCache::RecreateAllHBitmaps()
{
    CNullable<CD2D1Exception> nullable_d2d1exception{};

    TRAFFICMONITOR_CD2D1BITMAPCACHE_LOCK_CACHE_MAP_AND_EXPIRE_INTERVAL(m_sp_data);
    for (auto& cache : m_sp_data->m_cache_map)
    {
        auto hbitmap = cache.first;

        try
        {
            cache.second.Update(m_p_render_target, cache.first);
        }
        catch (CWICException& ex)
        {
            // 一般是HBITMAP失效导致的问题，直接忽略
            (void)ex;
        }
        catch (CD2D1Exception& ex)
        {
            // 只会保存最后一次异常
            nullable_d2d1exception.Construct(std::move(ex));
        }
    }
    if (nullable_d2d1exception)
    {
        throw nullable_d2d1exception.GetUnsafe();
    }
}

auto CD2D1BitmapCache::CreateD2D1BitmapFromHBitmap(ComPtr<ID2D1RenderTarget> p_render_target, HBITMAP hbitmap)
    -> ComPtr<ID2D1Bitmap>
{
    ComPtr<IWICBitmap> p_wic_bitmap{};
    ThrowIfFailed<CWICException>(
        CWICFactory::GetWIC()->CreateBitmapFromHBITMAP(
            hbitmap,
            NULL,
            WICBitmapUsePremultipliedAlpha,
            &p_wic_bitmap),
        "Call IWICImagingFactory::CreateBitmapFromHBITMAP failed.");

    ComPtr<ID2D1Bitmap> result{};
    ThrowIfFailed<CD2D1Exception>(
        p_render_target->CreateBitmapFromWicBitmap(
            p_wic_bitmap.Get(),
            &result),
        "Call ID2D1RenderTarget::CreateBitmapFromWicBitmap failed.");

    return result;
}

void CD2D1BitmapCache::GC()
{
    GCImpl(m_sp_data);
}

void CD2D1BitmapCache::SetGCInterval(const std::chrono::seconds interval)
{
    TRAFFICMONITOR_CD2D1BITMAPCACHE_LOCK_GC_INTERVAL(m_sp_data);
    m_sp_data->m_gc_interval = interval;
}

void CD2D1BitmapCache::SetExpireInterval(const std::chrono::seconds interval)
{
    TRAFFICMONITOR_CD2D1BITMAPCACHE_LOCK_CACHE_MAP_AND_EXPIRE_INTERVAL(m_sp_data);
    m_sp_data->m_cache_expire_interval = interval;
}

auto CD2D1BitmapCache::GetCachedBitmap(HBITMAP hbitmap)
    -> ComPtr<ID2D1Bitmap>
{
    TRAFFICMONITOR_CD2D1BITMAPCACHE_LOCK_CACHE_MAP_AND_EXPIRE_INTERVAL(m_sp_data);
    auto it = m_sp_data->m_cache_map.find(hbitmap);
    if (it != m_sp_data->m_cache_map.end())
    {
        if (m_sp_data->IsCacheExpire(it->second))
        {
            it->second.Update(m_p_render_target, it->first);
        }
        return it->second.m_cache;
    }
    else
    {
        AddHBitmap(hbitmap);
        return m_sp_data->m_cache_map[hbitmap].m_cache;
    }
}

bool CD2D1BitmapCache::IsHBitmapExist(HBITMAP hbitmap) const
{
    auto existing_it = m_sp_data->m_cache_map.find(hbitmap);
    if (existing_it == m_sp_data->m_cache_map.end())
    {
        return true;
    }
    return false;
}

void CD2D1BitmapCache::GCImpl(std::shared_ptr<HeapData> sp_data)
{
    auto now = std::chrono::steady_clock::now();
    TRAFFICMONITOR_CD2D1BITMAPCACHE_LOCK_CACHE_MAP_AND_EXPIRE_INTERVAL(sp_data);
    auto& ref_cache_map = sp_data->m_cache_map;
    for (auto it = ref_cache_map.begin(); it != ref_cache_map.end();)
    {
        if (sp_data->IsCacheExpire(it->second, now))
        {
            it = ref_cache_map.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void CTaskBarDlgDrawCommon::OnD3D10Exception1(CD3D10Exception1& ex)
{
    auto hr = ex.GetHResult();
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        m_p_window_support->RecreateDevice();
    }
    else
    {
        DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler(ex);
    }
}

void CTaskBarDlgDrawCommon::ResetClippedStateIfSet()
{
    if (m_is_clipped)
    {
        m_p_window_support
            ->GetRenderTarget()
            ->PopAxisAlignedClip();
    }
}

void CTaskBarDlgDrawCommon::Create(CTaskBarDlgDrawCommonWindowSupport& taskbar_dlg_draw_common_window_support, const D2D1_SIZE_U size)
{
    const static auto transparent_black =
        CRenderTarget::COLORREF_TO_D2DCOLOR(RGB(0, 0, 0), 0);
    const static auto gdi_back_color =
        CRenderTarget::COLORREF_TO_D2DCOLOR(RGB(0, 0, 0), DEFAULT_GDI_OP_TEXTURE_ALPHA);

    m_p_window_support = &taskbar_dlg_draw_common_window_support;
    m_p_window_support->Resize(size);

    auto p_render_target = taskbar_dlg_draw_common_window_support.GetRenderTarget();
    p_render_target->BeginDraw();
    p_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
    p_render_target->Clear(transparent_black);
}

auto CTaskBarDlgDrawCommon::Convert(CPoint point) noexcept
    -> D2D1_POINT_2F
{
    D2D1_POINT_2F result{
        static_cast<float>(point.x),
        static_cast<float>(point.y)};
    return result;
}

auto CTaskBarDlgDrawCommon::Convert(CRect rect) noexcept
    -> D2D1_RECT_F
{
    D2D1_RECT_F result{
        static_cast<float>(rect.left),
        static_cast<float>(rect.top),
        static_cast<float>(rect.right),
        static_cast<float>(rect.bottom)};

    return result;
}

CTaskBarDlgDrawCommon::~CTaskBarDlgDrawCommon()
{
    // Create的时候抛异常了
    if (!m_p_window_support)
    {
        return;
    }

    ResetClippedStateIfSet();
    auto p_render_target = m_p_window_support->GetRenderTarget();
    try
    {
        if (m_gdi_interop_object.HasValue())
        {
            auto texture_size = m_p_window_support->GetSize();
            auto data_size = texture_size.height * texture_size.width * 4;
            ::GdiFlush();
            auto& ref_gdi_interop_object = m_gdi_interop_object.Get();
            m_p_window_support->InitGdiInteropTexture(
                ref_gdi_interop_object.m_p_gdi_interop_hbitmap_data, data_size);
            auto waiter = m_p_window_support->DrawAlphaValueReduceEffect();

            p_render_target->Flush();

            ThrowIfFailed<CD3D10Exception1>(
                waiter.Wait(),
                "Draw call failed.");

            D2D1_BITMAP_PROPERTIES d2d1_bitmap_properties{};
            d2d1_bitmap_properties.pixelFormat.format = CTaskBarDlgDrawCommonWindowSupport::PIXEL_FORMAT;
            d2d1_bitmap_properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
            d2d1_bitmap_properties.dpiX = CTaskBarDlgDrawCommonWindowSupport::DEFAULT_DPI;
            d2d1_bitmap_properties.dpiY = CTaskBarDlgDrawCommonWindowSupport::DEFAULT_DPI;
            auto p_gdi_interop_surface1 = m_p_window_support->GetGdiFinalTextureSurface();
            ComPtr<ID2D1Bitmap> p_gdi_interop_bitmap{};
            ThrowIfFailed<CD2D1Exception>(
                p_render_target->CreateSharedBitmap(
                    __uuidof(IDXGISurface1),
                    p_gdi_interop_surface1.Get(),
                    &d2d1_bitmap_properties,
                    &p_gdi_interop_bitmap),
                "Create ID2D1Bitmap from IDXGISurface1 failed.");
            p_render_target->DrawBitmap(p_gdi_interop_bitmap.Get());
        }
        ThrowIfFailed<CD2D1Exception>(
            p_render_target->EndDraw(),
            "Call ID2D1RenderTarget::EndDraw() failed.");
    }
    catch (CD3D10Exception1& ex)
    {
        OnD3D10Exception1(ex);
    }
    catch (CHResultException& ex)
    {
        DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler(ex);
    }
}

void CTaskBarDlgDrawCommon::SetBackColor(COLORREF back_color, BYTE alpha)
{
    m_p_window_support->SetBackColor(back_color, alpha);
}

void CTaskBarDlgDrawCommon::SetFont(CFont* pfont)
{
    m_p_window_support->SetFont(*pfont);
}

void CTaskBarDlgDrawCommon::DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align, bool draw_back_ground, bool multi_line, BYTE alpha)
{
    auto length = ::wcslen(lpszString);
    auto length_u = static_cast<UINT>(length);
    auto p_text_format = m_p_window_support->GetTextFormat();
    // 备份状态
    auto old_vertical_align = p_text_format->GetParagraphAlignment();
    auto old_horizontal_align = p_text_format->GetTextAlignment();
    auto old_word_warpping = p_text_format->GetWordWrapping();
    // GDI版本的DrawWindowText文字对齐处理没看明白
    if (multi_line)
    {
        // DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX
        p_text_format->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
    }
    else
    {
        // DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX
        p_text_format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);            // DT_SINGLELINE
        p_text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); // 此处仅设置竖直方向的中间对齐
    }
    switch (align)
    {
    case Alignment::LEFT:
    {
        p_text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        break;
    }
    case Alignment::CENTER:
    {
        p_text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        break;
    }
    case Alignment::RIGHT:
    {
        p_text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        break;
    }
    };

    auto layout_rect = Convert(rect);
    ComPtr<IDWriteTextLayout> p_text_layout{};
    ThrowIfFailed<CDWriteException>(
        DWriteSupport::GetFactory()->CreateTextLayout(
            lpszString,
            length_u,
            p_text_format.Get(),
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
        m_p_window_support
            ->GetRenderTarget()
            ->FillRectangle(rect_f, m_p_window_support->GetRawBackSolidColorBruch());
    }

    m_p_window_support
        ->SetForeColor(color, alpha)
        ->GetRenderTarget()
        ->DrawTextLayout(
            {layout_rect.left, layout_rect.top},
            p_text_layout.Get(),
            m_p_window_support->GetRawForeSolidColorBruch(),
            D2D1_DRAW_TEXT_OPTIONS_NO_SNAP | D2D1_DRAW_TEXT_OPTIONS_CLIP); // 不允许文字超出边界
    // 恢复状态
    p_text_format->SetParagraphAlignment(old_vertical_align);
    p_text_format->SetTextAlignment(old_horizontal_align);
    p_text_format->SetWordWrapping(old_word_warpping);
}

void CTaskBarDlgDrawCommon::SetDrawRect(CRect rect)
{
    ResetClippedStateIfSet();
    D2D1_RECT_F d2d1_clip_rect;
    d2d1_clip_rect.left = static_cast<float>(rect.left);
    d2d1_clip_rect.top = static_cast<float>(rect.top);
    d2d1_clip_rect.right = static_cast<float>(rect.right);
    d2d1_clip_rect.bottom = static_cast<float>(rect.bottom);
    m_p_window_support
        ->GetRenderTarget()
        ->PushAxisAlignedClip(d2d1_clip_rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    m_is_clipped = true;
}

void CTaskBarDlgDrawCommon::FillRect(CRect rect, COLORREF color, BYTE alpha)
{
    auto rect_f = Convert(rect);
    m_p_window_support
        ->SetForeColor(color, alpha)
        ->GetRenderTarget()
        ->FillRectangle(rect_f, m_p_window_support->GetRawForeSolidColorBruch());
}

void CTaskBarDlgDrawCommon::DrawRectOutLine(CRect rect, COLORREF color, int width, bool dot_line, BYTE alpha)
{
    rect.DeflateRect(width / 2, width / 2);
    auto rect_f = Convert(rect);
    m_p_window_support->SetForeColor(color, alpha);
    auto width_f = static_cast<float>(width);
    if (dot_line)
    {
        m_p_window_support
            ->GetRenderTarget()
            ->DrawRectangle(
                rect_f,
                m_p_window_support->GetRawForeSolidColorBruch(),
                width_f,
                m_p_window_support->GetRawPsDotLikeStyle());
    }
    else
    {
        m_p_window_support
            ->GetRenderTarget()
            ->DrawRectangle(
                rect_f,
                m_p_window_support->GetRawForeSolidColorBruch(),
                width_f);
    }
}

void CTaskBarDlgDrawCommon::DrawLine(CPoint start_point, int height, COLORREF color, BYTE alpha)
{
    auto d2d1_start_point = Convert(start_point);
    D2D1_POINT_2F d2d1_end_point{d2d1_start_point.x, d2d1_start_point.y - height};
    m_p_window_support
        ->SetForeColor(color, alpha)
        ->GetRenderTarget()
        ->DrawLine(
            d2d1_start_point,
            d2d1_end_point,
            m_p_window_support->GetRawForeSolidColorBruch());
}

void CTaskBarDlgDrawCommon::SetTextColor(const COLORREF color, BYTE alpha)
{
    m_text_color = color;
    m_p_window_support->SetForeColor(color, alpha);
}

void CTaskBarDlgDrawCommon::DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode, BYTE alpha)
{
    auto p_d2d1_bitmap = m_p_window_support->GetCachedBitmap(hbitmap);
    if (!p_d2d1_bitmap)
    {
        CD2D1BitmapCache::CreateD2D1BitmapFromHBitmap(m_p_window_support->GetRenderTarget(), hbitmap);
    }
    auto p_render_target = m_p_window_support->GetRenderTarget();

    float opacity = static_cast<float>(alpha) / 255.f;
    D2D1_RECT_F draw_rect_f;
    if (size.cx == 0 || size.cy == 0)
    {
        auto bitmap_size = p_d2d1_bitmap->GetSize();
        draw_rect_f.left = static_cast<float>(start_point.x);
        draw_rect_f.top = static_cast<float>(start_point.y);
        draw_rect_f.right = static_cast<float>(start_point.x) + bitmap_size.width;
        draw_rect_f.bottom = static_cast<float>(start_point.y) + bitmap_size.height;
        p_render_target->DrawBitmap(p_d2d1_bitmap.Get(), draw_rect_f, opacity);
        return;
    }
    switch (stretch_mode)
    {
    case StretchMode::STRETCH:
    {
        draw_rect_f.left = static_cast<float>(start_point.x);
        draw_rect_f.top = static_cast<float>(start_point.y);
        draw_rect_f.right = static_cast<float>(start_point.x + size.cx);
        draw_rect_f.bottom = static_cast<float>(start_point.y + size.cy);
        p_render_target->DrawBitmap(p_d2d1_bitmap.Get(), draw_rect_f, opacity);
        return;
    }
    case StretchMode::FILL:
    {
        auto draw_size = size;
        SetDrawRect(CRect(start_point, draw_size));

        auto bitmap_size = p_d2d1_bitmap->GetPixelSize();
        float bitmap_aspect_ratio, draw_rect_acpect_ratio; // 图像的宽高比、绘制大小的宽高比
        bitmap_aspect_ratio = static_cast<float>(bitmap_size.width) / static_cast<float>(bitmap_size.height);
        draw_rect_acpect_ratio = static_cast<float>(size.cx) / size.cy;
        if (bitmap_aspect_ratio > draw_rect_acpect_ratio) // 如果图像的宽高比大于绘制区域的宽高比，则需要裁剪两边的图像
        {
            int image_width; // 按比例缩放后的宽度
            image_width = bitmap_size.width * draw_size.cy / bitmap_size.height;
            start_point.x -= ((image_width - draw_size.cx) / 2);
            draw_size.cx = image_width;
        }
        else
        {
            int image_height; // 按比例缩放后的高度
            image_height = bitmap_size.height * draw_size.cx / bitmap_size.width;
            start_point.y -= ((image_height - draw_size.cy) / 2);
            draw_size.cy = image_height;
        }
        break;
    }
    case StretchMode::FIT:
    {
        auto draw_size = p_d2d1_bitmap->GetPixelSize();
        // 图像宽高比
        float bitmap_aspect_ratio = static_cast<float>(draw_size.width) / static_cast<float>(draw_size.height);
        float draw_rect_acpect_ratio = static_cast<float>(size.cx) / static_cast<float>(size.cy);
        if (bitmap_aspect_ratio > draw_rect_acpect_ratio) // 如果图像的宽高比大于绘制区域的宽高比
        {
            draw_size.height = draw_size.height * size.cx / draw_size.width;
            draw_size.width = size.cx;
            start_point.y += ((size.cy - draw_size.height) / 2);
        }
        else
        {
            draw_size.width = draw_size.width * size.cy / draw_size.height;
            draw_size.height = size.cy;
            start_point.x += ((size.cx - draw_size.width) / 2);
        }
        break;
    }
    default:
        break;
    }
    draw_rect_f.left = static_cast<float>(start_point.x);
    draw_rect_f.top = static_cast<float>(start_point.y);
    draw_rect_f.right = draw_rect_f.left + size.cx;
    draw_rect_f.bottom = draw_rect_f.top + size.cy;
    p_render_target->DrawBitmap(p_d2d1_bitmap.Get(), draw_rect_f, opacity);
}

CDC* CTaskBarDlgDrawBuffer::GetMemDC()
{
    return nullptr;
}

void CTaskBarDlgDrawBuffer::SetTargetSurface(Microsoft::WRL::ComPtr<IDXGISurface1> p_surface) noexcept
{
    m_p_gdi_surface = p_surface;
}

auto CTaskBarDlgDrawBuffer::GetDefaultBlendFunctionPointer() noexcept
    -> const ::PBLENDFUNCTION
{
    static ::BLENDFUNCTION result{
        AC_SRC_OVER,
        0,
        0xFF,
        AC_SRC_ALPHA};
    return &result;
}

CTaskBarDlgDrawBuffer::CTaskBarDlgDrawBuffer(CSize size, HWND hwnd)
    : m_size{size}, m_target_hwnd{hwnd}
{
}

CTaskBarDlgDrawBuffer::~CTaskBarDlgDrawBuffer()
{
    // 类在初始化之前就被抛异常了
    if (!m_p_gdi_surface)
    {
        return;
    }

    POINT start_location{0, 0};
    HDC gdi_dc{NULL};
    ThrowIfFailed<CD3D10Exception1>(
        m_p_gdi_surface->GetDC(FALSE, &gdi_dc),
        "Get DC from IDXGISurface1 failed.");
    UPDATELAYEREDWINDOWINFO update_window_info;
    ::memset(&update_window_info, 0, sizeof(update_window_info));
    update_window_info.cbSize = sizeof(update_window_info);
    // update_window_info.hdcDst = NULL;
    // update_window_info.pptDst = NULL; 不更新窗口位置
    update_window_info.psize = &m_size;
    update_window_info.hdcSrc = gdi_dc;
    update_window_info.pptSrc = &start_location;
    // update_window_info.crKey = 0;
    update_window_info.pblend = GetDefaultBlendFunctionPointer();
    update_window_info.dwFlags = ULW_ALPHA;
    // m_update_window_info.prcDirty = NULL;
    BOOL state = ::UpdateLayeredWindowIndirect(m_target_hwnd, &update_window_info);
    if (state == 0)
    {
        auto error_code = ::GetLastError();
        LogWin32ApiErrorMessage();
        CString error_info{};
        error_info.Format(_T("Call UpdateLayeredWindowIndirect failed. Use GDI render instead. Error code = %ld."), error_code);
        CCommon::WriteLog(error_info, theApp.m_log_path.c_str());

        // 禁用D2D
        theApp.m_taskbar_data.disable_d2d = true;
        // 展示错误信息
        ::MessageBox(NULL, CCommon::LoadText(IDS_UPDATE_TASKBARDLG_FAILED_TIP), NULL, MB_OK | MB_ICONWARNING);
    }
    RECT empty_rect{};
    m_p_gdi_surface->ReleaseDC(&empty_rect);
}

CTaskBarDlgDrawCommon::CGdiInteropObject::CGdiInteropObject(D2D1_SIZE_U size)
{
    m_gdi_interop_cdc.CreateCompatibleDC(NULL);
    auto bitmap_info = TaskBarDlgUser32DrawTextHook::Details::GetArgb32BitmapInfo(size.width, size.height);
    m_gdi_interop_hbitmap = ::CreateDIBSection(
        m_gdi_interop_cdc,
        &bitmap_info,
        DIB_RGB_COLORS,
        &m_p_gdi_interop_hbitmap_data,
        NULL,
        0);
    m_gdi_interop_old_hbitmap = m_gdi_interop_cdc.SelectObject(m_gdi_interop_hbitmap);
    ::SetBkColor(m_gdi_interop_cdc, TRANSPARENT);
    auto gdi_no_modified_alpha_dc = DrawCommonHelper::Get1x1AlphaEqual1DC();
    ::BLENDFUNCTION blend_function{
        AC_SRC_OVER,
        0,
        0xFF,
        AC_SRC_ALPHA};
    ::AlphaBlend(
        m_gdi_interop_cdc,
        0, 0,
        size.width, size.height,
        gdi_no_modified_alpha_dc,
        0, 0,
        1, 1,
        blend_function);
}

CTaskBarDlgDrawCommon::CGdiInteropObject::~CGdiInteropObject()
{
    m_gdi_interop_cdc.SelectObject(m_gdi_interop_old_hbitmap);
    ::DeleteObject(m_gdi_interop_hbitmap);
}
