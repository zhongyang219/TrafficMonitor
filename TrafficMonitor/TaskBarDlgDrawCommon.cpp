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

void LogWin32ApiErrorMessage(DWORD error_code) noexcept
{
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

std::uint32_t DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler::m_error_count = 0;

#define TRAFFICMONITOR_ENUM_AND_EXPLANATION(enumeration, explanation) \
    enumeration, "Handling device loss. HResult = " #enumeration ": " explanation

void DrawCommonHelper::LogDeviceRecreateReason(HRESULT hr)
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

    auto it = removed_reason.find(hr);
    if (it != removed_reason.end())
    {
        CCommon::WriteLog(it->second, theApp.m_log_path.c_str());
    }
    else
    {
        constexpr static auto unknown_reason{"Recreate the device for an unknown reason."};
        CCommon::WriteLog(unknown_reason, theApp.m_log_path.c_str());
        auto str_hr = std::string{"The reason reports HResult = "} + std::to_string(hr);
        CCommon::WriteLog(str_hr.c_str(), theApp.m_log_path.c_str());
    }
}

DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler::DefaultD2DDrawCommonExceptionHandler(CHResultException& ex) noexcept
    : m_ref_ex(ex)
{
    ++m_error_count;
}

void DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler::ResetErrorCount()
{
    m_error_count = 0;
}

void DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler::HandleErrorCountIncrement()
{
    if (m_error_count > DrawCommonHelper::MAX_D2D1_RENDER_ERROR_COUNT)
    {
        m_error_count = 0;
        // 禁用D2D绘图
        theApp.m_taskbar_data.disable_d2d = true;
        ::MessageBox(NULL, CCommon::LoadText(IDS_D2DDRAWCOMMON_ERROR_TIP), NULL, MB_OK | MB_ICONWARNING);
    }
}

void DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler::IncreaseErrorCountManually()
{
    ++m_error_count;
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
                throw std::runtime_error{
                    TRAFFICMONITOR_ERROR_STR("Create ARGB32 DIB (size = 1x1, alpha = 1) failed.")};
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

bool CTaskBarDlgDrawCommonSupport::IsAllDevicesRecreatedByThisFunction()
{
    auto p_d3d10_device1 = m_d3d10_device1.Get();
    auto hr = p_d3d10_device1->GetDeviceRemovedReason();
    if (hr != S_OK)
    {
        CCommon::WriteLog("Notice: D3D10.1 device is invalid. All devices will be recreated. This message is sent by "
                          "the function on the next line:",
                          theApp.m_log_path.c_str());
        CCommon::WriteLog(__FUNCSIG__, theApp.m_log_path.c_str());
        RecreateAll();
        return true;
    }
    return false;
}

void CTaskBarDlgDrawCommonSupport::InternalRecreateD3D10Device1()
{
    auto &&default_adapter1 = CD3D10Support1::GetDeviceList(true).front();
    m_d3d10_device1.Recreate(default_adapter1);
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
        CD2D1Support1::GetFactory()->CreateStrokeStyle(
            d2d1_stroke_style_properties,
            NULL,
            0,
            &m_p_ps_dot_like_style),
        TRAFFICMONITOR_ERROR_STR("Create GDI PS_DOT like stroke style failed."));

    UINT flag = D3D10_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef DEBUG
    flag |= D3D10_CREATE_DEVICE_DEBUG;
#endif
    m_d3d10_device1.SetFlags(flag);
    RecreateAll();
}

auto CTaskBarDlgDrawCommonSupport::GetD3D10Device1() noexcept
    -> CD3D10Device1&
{
    return m_d3d10_device1;
}

auto CTaskBarDlgDrawCommonSupport::GetD2D1Device() noexcept
    -> CD2D1Device&
{
    return m_d2d1_device;
}

auto CTaskBarDlgDrawCommonSupport::GetDCompositionDevice() noexcept
    -> CDCompositionDevice&
{
    return m_dcomposition_device;
}

auto CTaskBarDlgDrawCommonSupport::GetPsDotLikeStyle() noexcept
    -> Microsoft::WRL::ComPtr<ID2D1StrokeStyle>
{
    return m_p_ps_dot_like_style;
}

void CTaskBarDlgDrawCommonSupport::RecreateAll()
{
    CCommon::WriteLog("Notice: All devices will be recreated. This message is sent by the function on the next line:",
                      theApp.m_log_path.c_str());
    CCommon::WriteLog(__FUNCSIG__, theApp.m_log_path.c_str());
    InternalRecreateD3D10Device1();
    RecreateD2D1Device();
    RecreateDCompositionDevice();
}

void CTaskBarDlgDrawCommonSupport::RecreateD3D10Device1(const HRESULT recreate_reason)
{
    if (recreate_reason != S_OK)
    {
        DrawCommonHelper::LogDeviceRecreateReason(recreate_reason);
        CCommon::WriteLog("Notice: D3D10.1 device is invalid. All devices will be recreated. This message is sent by "
                          "the function on the next line:",
                          theApp.m_log_path.c_str());
        CCommon::WriteLog(__FUNCSIG__, theApp.m_log_path.c_str());
    }
    RecreateAll();
}

void CTaskBarDlgDrawCommonSupport::RecreateD2D1Device(const HRESULT recreate_reason)
{
    if (recreate_reason != S_OK)
    {
        DrawCommonHelper::LogDeviceRecreateReason(recreate_reason);
        CCommon::WriteLog(
            "Notice: The D2D1 device will be recreated. This message is sent by the function on the next line:",
            theApp.m_log_path.c_str());
        CCommon::WriteLog(__FUNCSIG__, theApp.m_log_path.c_str());
    }
    if (IsAllDevicesRecreatedByThisFunction())
    {
        return;
    }
    Microsoft::WRL::ComPtr<IDXGIDevice> p_dxgi_device{};
    ThrowIfFailed<CD3D10Exception1>(
        m_d3d10_device1.Get()->QueryInterface(IID_PPV_ARGS(&p_dxgi_device)),
        "Get IDXGIDevice form ID3D10Device1 failed.");
    m_d2d1_device.Recreate(p_dxgi_device);
}

void CTaskBarDlgDrawCommonSupport::RecreateDCompositionDevice(const HRESULT recreate_reason)
{
    if (recreate_reason != S_OK)
    {
        DrawCommonHelper::LogDeviceRecreateReason(recreate_reason);
        CCommon::WriteLog("Notice: The DirectComposition device will be recreated. This message is sent by the function on the next line:",
            theApp.m_log_path.c_str());
        CCommon::WriteLog(__FUNCSIG__, theApp.m_log_path.c_str());
    }
    if (IsAllDevicesRecreatedByThisFunction())
    {
        return;
    }
    auto p_d3d10_device1 = m_d3d10_device1.Get();
    if (p_d3d10_device1)
    {
        ComPtr<IDXGIDevice> p_dxgi_device{};
        ThrowIfFailed<CD3D10Exception1>(
            p_d3d10_device1.As(&p_dxgi_device),
            TRAFFICMONITOR_ERROR_STR("Get dxgi device from d3d10 device1 failed."));
        m_dcomposition_device.Recreate(p_dxgi_device);
        return;
    }
    throw std::runtime_error{TRAFFICMONITOR_ERROR_STR("D3D10 device not available.")};
}

bool CTaskBarDlgDrawCommonSupport::CheckSupport() noexcept
{
    return CD2D1Support1::CheckSupport() && CDWriteSupport::CheckSupport();
}

auto CTaskBarDlgDrawCommonWindowSupport::CD3DGdiInteropHelper::CreateDefaultTexture(ComPtr<ID3D10Device1> p_device1, D2D1_SIZE_U size)
    -> Microsoft::WRL::ComPtr<ID3D10Texture2D>
{
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

    ComPtr<ID3D10Texture2D> result{};
    ThrowIfFailed<CD3D10Exception1>(
        p_device1->CreateTexture2D(
            &description,
            NULL,
            &result),
        TRAFFICMONITOR_ERROR_STR("Create default 2D texture failed."));

    return result;
}

auto CTaskBarDlgDrawCommonWindowSupport::CD3DGdiInteropHelper::CreateCpuWriteableTexture(Microsoft::WRL::ComPtr<ID3D10Device1> p_device1, const D2D1_SIZE_U size)
    -> Microsoft::WRL::ComPtr<ID3D10Texture2D>
{
    D3D10_TEXTURE2D_DESC description = {};
    description.ArraySize = 1;
    description.Format = PIXEL_FORMAT;
    description.Width = size.width;
    description.Height = size.height;
    description.MipLevels = 1;
    description.SampleDesc.Count = 1;
    description.BindFlags = D3D10_BIND_SHADER_RESOURCE;
    description.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    description.Usage = D3D10_USAGE_DYNAMIC;

    ComPtr<ID3D10Texture2D> result{};
    ThrowIfFailed<CD3D10Exception1>(
        p_device1->CreateTexture2D(
            &description,
            NULL,
            &result),
        TRAFFICMONITOR_ERROR_STR("Create CPU writeable 2D texture failed."));

    return result;
}

auto CTaskBarDlgDrawCommonWindowSupport::CD3DGdiInteropHelper::GetPsAlphaValueReducer() noexcept
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

CTaskBarDlgDrawCommonWindowSupport::CD3DGdiInteropHelper::CD3DGdiInteropHelper(CD3D10Device1& ref_device)
    : Base{ref_device.GetStorage()}, m_p_device1{ref_device.Get()}, m_alpha_value_effect{ref_device.Get()}
{
}

void CTaskBarDlgDrawCommonWindowSupport::CD3DGdiInteropHelper::OnDeviceRecreate(DeviceType p_new_device) noexcept
{
    m_p_device1 = p_new_device;
    m_is_p_device1_recreated = true;
}

bool CTaskBarDlgDrawCommonWindowSupport::CD3DGdiInteropHelper::HandleDeviceRecreationIfNecessary()
{
    if (m_is_p_device1_recreated)
    {
        m_alpha_value_effect.RebindDevice1(m_p_device1);
        return std::exchange(m_is_p_device1_recreated, false);
    }
    return false;
}

void CTaskBarDlgDrawCommonWindowSupport::CD3DGdiInteropHelper::Resize(D2D1_SIZE_U size)
{
    m_p_gdi_initial_texture = CreateCpuWriteableTexture(m_p_device1, size);
    ThrowIfFailed<CD3D10Exception1>(
        m_p_gdi_initial_texture->QueryInterface(IID_PPV_ARGS(&m_p_gdi_initial_surface)),
        TRAFFICMONITOR_ERROR_STR("Get IDXGISurface1 from D2D gdi_initial_texture failed."));

    m_p_gdi_final_texture = CreateDefaultTexture(m_p_device1, size);
    ThrowIfFailed<CD3D10Exception1>(
        m_p_gdi_final_texture->QueryInterface(IID_PPV_ARGS(&m_p_gdi_final_surface)),
        TRAFFICMONITOR_ERROR_STR("Get IDXGISurface1 from D2D gdi_final_texture failed."));

    m_alpha_value_effect
        .SetOutputSize(size.width, size.height)
        .SetPsByteCode(GetPsAlphaValueReducer());
}

CTaskBarDlgDrawCommonWindowSupport::CTaskBarDlgDrawCommonWindowSupport(CTaskBarDlgDrawCommonSupport& ref_taskbdlg_draw_common_support)
    : m_d3d_gdi_interop_helper{ref_taskbdlg_draw_common_support.GetD3D10Device1()},
      m_ref_taskbardlg_draw_common_support{ref_taskbdlg_draw_common_support}
{
}

void CTaskBarDlgDrawCommonWindowSupport::Resize(const D2D1_SIZE_U size)
{
    auto force_refresh = m_d3d_gdi_interop_helper.HandleDeviceRecreationIfNecessary();
    if (size.width != m_size.width || size.height != m_size.height || force_refresh)
    {
        m_size = size;
        m_is_size_updated = true;
    }
}

void CTaskBarDlgDrawCommonWindowSupport::ResizeGdiInteropPart()
{
    if (m_is_size_updated)
    {
        m_d3d_gdi_interop_helper.Resize(m_size);
        m_is_size_updated = false;
    }
}

void CTaskBarDlgDrawCommonWindowSupport::SetGdiInteropTexture(void* p_data, D2D1_SIZE_U size)
{
    constexpr auto RGBA_SIZE = 4;

    D3D10_MAPPED_TEXTURE2D mapped_texture{};
    ThrowIfFailed<CD3D10Exception1>(
        m_d3d_gdi_interop_helper.m_p_gdi_initial_texture->Map(
            0,
            D3D10_MAP_WRITE_DISCARD,
            0,
            &mapped_texture),
        TRAFFICMONITOR_ERROR_STR("Map m_p_gdi_initial_texture failed."));

    auto p_gpu_data = mapped_texture.pData;
    auto input_pitch = size.width * RGBA_SIZE;
    if (mapped_texture.RowPitch == input_pitch)
    {
        std::size_t data_size = input_pitch * size.height;
        std::memcpy(p_gpu_data, p_data, data_size);
    }
    else
    {
        auto p_from = static_cast<BYTE*>(p_data);
        auto p_to = static_cast<BYTE*>(p_gpu_data);
        for (auto i = 0u; i < size.height; ++i)
        {
            std::memcpy(p_to, p_from, input_pitch);
            std::advance(p_from, input_pitch);
            std::advance(p_to, mapped_texture.RowPitch);
        }
    }

    m_d3d_gdi_interop_helper.m_p_gdi_initial_texture->Unmap(0);
}

void CTaskBarDlgDrawCommonWindowSupport::SetFont(HFONT h_font)
{
    m_dwrite_helper.SetFont(h_font);
}

auto CTaskBarDlgDrawCommonWindowSupport::GetTextFormat() noexcept
    -> Microsoft::WRL::ComPtr<IDWriteTextFormat>
{
    return m_dwrite_helper.GetTextFormat();
}

auto CTaskBarDlgDrawCommonWindowSupport::GetRawPsDotLikeStyle() noexcept -> ID2D1StrokeStyle*
{
    return m_ref_taskbardlg_draw_common_support.GetPsDotLikeStyle().Get();
}

auto CTaskBarDlgDrawCommonWindowSupport::GetGdiInitalTextureSurface() noexcept
    -> Microsoft::WRL::ComPtr<IDXGISurface1>
{
    return m_d3d_gdi_interop_helper.m_p_gdi_initial_surface;
}

auto CTaskBarDlgDrawCommonWindowSupport::GetGdiFinalTextureSurface() noexcept
    -> Microsoft::WRL::ComPtr<IDXGISurface1>
{
    return m_d3d_gdi_interop_helper.m_p_gdi_final_surface;
}

auto CTaskBarDlgDrawCommonWindowSupport::GetSize() const noexcept
    -> D2D1_SIZE_U
{
    return m_size;
}

auto CTaskBarDlgDrawCommonWindowSupport::GetFont() noexcept
    -> HFONT
{
    return m_dwrite_helper.GetFont();
}

auto CTaskBarDlgDrawCommonWindowSupport::DrawAlphaValueReduceEffect() noexcept
    -> CD3D10DrawCallWaiter
{
    constexpr std::array<FLOAT, 4> transparent_black{.0f, .0f, .0f, .0f};

    m_d3d_gdi_interop_helper.m_alpha_value_effect
        .SetOutputTexture(m_d3d_gdi_interop_helper.m_p_gdi_final_texture)
        .ClearOnly(transparent_black);
    return m_d3d_gdi_interop_helper.m_alpha_value_effect
        .SetInputTexture(m_d3d_gdi_interop_helper.m_p_gdi_initial_texture)
        .ApplyPipelineConfig()
        .Draw();
}

void CTaskBarDlgDrawCommonWindowSupport::RequestD3D10Device1Recreate()
{
    auto hr = m_d3d_gdi_interop_helper.m_p_device1->GetDeviceRemovedReason();
    m_ref_taskbardlg_draw_common_support.RecreateD3D10Device1(hr);
}

void CTaskBarDlgDrawCommonWindowSupport::CDWriteHelper::SetFont(HFONT h_font)
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
            CDWriteSupport::GetFactory()->CreateTextFormat(
                theApp.m_taskbar_data.font.name,
                NULL,
                dwrite_font_weight,
                dwrite_font_style,
                DWRITE_FONT_STRETCH_NORMAL,
                abs_font_height_f,
                L"",
                &m_p_text_format),
            TRAFFICMONITOR_ERROR_STR("Create D2D1 Text Format failed."));

        m_h_last_font = h_font;
    }
}

auto CTaskBarDlgDrawCommonWindowSupport::CDWriteHelper::GetFont() noexcept
    -> HFONT
{
    return m_h_last_font;
}

auto CTaskBarDlgDrawCommonWindowSupport::CDWriteHelper::GetTextFormat() noexcept
    -> ComPtr<IDWriteTextFormat>
{
    return m_p_text_format;
}

void CD2D1DeviceContextWindowSupport::CD2D1DeviceContextHelper::Reinitialize()
{
    constexpr static D2D1_DEVICE_CONTEXT_OPTIONS options = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;

    ThrowIfFailed<CD2D1Exception>(
        m_p_device->CreateDeviceContext(
            options,
            &m_p_device_context),
        TRAFFICMONITOR_ERROR_STR("Create D2D1 device context failed."));
    ThrowIfFailed<CD2D1Exception>(
        m_p_device_context->CreateSolidColorBrush(
            m_foreground_color,
            &m_p_foreground_color_brush),
        TRAFFICMONITOR_ERROR_STR("Create foreground solid color brush failed."));
    ThrowIfFailed<CD2D1Exception>(
        m_p_device_context->CreateSolidColorBrush(
            m_background_color,
            &m_p_background_color_brush),
        TRAFFICMONITOR_ERROR_STR("Create background solid color brush failed."));

    auto sp_bitmap_cache = m_wp_bitmap_cache.lock();
    if (sp_bitmap_cache)
    {
        sp_bitmap_cache->RebindRenderTarget(m_p_device_context);
    }
}

CD2D1DeviceContextWindowSupport::CD2D1DeviceContextHelper::CD2D1DeviceContextHelper(CD2D1Device& ref_device)
    : Base{ref_device.GetStorage()}, m_p_device{ref_device.Get()}
{
}

bool CD2D1DeviceContextWindowSupport::CD2D1DeviceContextHelper::HandleDeviceRecreationIfNecessary()
{
    if (m_need_recreate)
    {
        Reinitialize();
        return std::exchange(m_need_recreate, false);
    }
    return false;
}

auto CD2D1DeviceContextWindowSupport::CD2D1DeviceContextHelper::GetRawForeSolidColorBruch() noexcept
    -> ID2D1SolidColorBrush*
{
    return m_p_foreground_color_brush.Get();
}

auto CD2D1DeviceContextWindowSupport::CD2D1DeviceContextHelper::GetRawBackSolidColorBruch() noexcept
    -> ID2D1SolidColorBrush*
{
    return m_p_background_color_brush.Get();
}

void CD2D1DeviceContextWindowSupport::CD2D1DeviceContextHelper::SetBackColor(COLORREF color, BYTE alpha) noexcept
{
    auto new_d2d1_color = CRenderTarget::COLORREF_TO_D2DCOLOR(color, alpha);
    if (!IsBitwiseEquality(new_d2d1_color, m_background_color))
    {
        m_p_background_color_brush->SetColor(new_d2d1_color);
        m_background_color = new_d2d1_color;
    };
}

void CD2D1DeviceContextWindowSupport::CD2D1DeviceContextHelper::SetForeColor(COLORREF color, BYTE alpha) noexcept
{
    auto new_d2d1_color = CRenderTarget::COLORREF_TO_D2DCOLOR(color, alpha);
    if (!IsBitwiseEquality(new_d2d1_color, m_foreground_color))
    {
        m_p_foreground_color_brush->SetColor(new_d2d1_color);
        m_foreground_color = new_d2d1_color;
    };
}

void CD2D1DeviceContextWindowSupport::CD2D1DeviceContextHelper::OnDeviceRecreate(DeviceType p_new_device) noexcept
{
    m_p_device = p_new_device;
    m_need_recreate = true;
}

void CD2D1DeviceContextWindowSupport::CD2D1DeviceContextHelper::RebindD2D1BitmapCache(std::weak_ptr<class CD2D1BitmapCache> wp_cache)
{
    m_wp_bitmap_cache = wp_cache;
}

void CD2D1DeviceContextWindowSupport::CD2D1DeviceContextHelper::RebindRenderTargetSurface(Microsoft::WRL::ComPtr<IDXGISurface> p_surface)
{
    const D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.f,
        96.f);

    if (!m_p_device_context)
    {
        Reinitialize();
    }
    ThrowIfFailed<CD2D1Exception>(
        m_p_device_context->CreateBitmapFromDxgiSurface(
            p_surface.Get(),
            &bitmap_properties,
            &m_p_d2d_render_target_bitmap),
        TRAFFICMONITOR_ERROR_STR("Create ID2D1Bitmap1 from dxgi surface failed."));
    m_p_device_context->SetTarget(m_p_d2d_render_target_bitmap.Get());
}

void CD2D1DeviceContextWindowSupport::CD2D1DeviceContextHelper::Reset()
{
    m_p_device_context.Reset();
    m_p_d2d_render_target_bitmap.Reset();
    m_p_foreground_color_brush.Reset();
    m_p_background_color_brush.Reset();
}

auto CD2D1DeviceContextWindowSupport::CD2D1DeviceContextHelper::GetCachedBitmap(HBITMAP hbitmap) const
    -> Microsoft::WRL::ComPtr<ID2D1Bitmap>
{
    auto sp_cache = m_wp_bitmap_cache.lock();
    if (sp_cache)
    {
        auto result = sp_cache->GetCachedBitmap(hbitmap);
        return result;
    }
    return {};
}

auto CD2D1DeviceContextWindowSupport::CD2D1DeviceContextHelper::GetDeviceContext() const
    -> Microsoft::WRL::ComPtr<ID2D1DeviceContext>
{
    return m_p_device_context;
}

void CD2D1DeviceContextWindowSupport::CDCompositionHelper::InitializeSwapChain(const D2D1_SIZE_U size)
{
    DXGI_SWAP_CHAIN_DESC1 dxgi_swap_chain_desc1;
    ::memset(&dxgi_swap_chain_desc1, 0, sizeof(dxgi_swap_chain_desc1));
    dxgi_swap_chain_desc1.Width = size.width;
    dxgi_swap_chain_desc1.Height = size.height;
    dxgi_swap_chain_desc1.Format = CTaskBarDlgDrawCommonWindowSupport::PIXEL_FORMAT;
    dxgi_swap_chain_desc1.SampleDesc.Count = 1;
    dxgi_swap_chain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    dxgi_swap_chain_desc1.BufferCount = 2;
    dxgi_swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    dxgi_swap_chain_desc1.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

    ThrowIfFailed<CDxgiException>(
        CDxgi1Support2::GetFactory()->CreateSwapChainForComposition(
            m_p_d3d10_device1.Get(),
            &dxgi_swap_chain_desc1,
            nullptr,
            &m_p_swap_chain1),
        TRAFFICMONITOR_ERROR_STR("Create swap chain for composition failed."));
}

void CD2D1DeviceContextWindowSupport::CDCompositionHelper::RebindSwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain1> p_swap_chain1)
{
    m_p_visual->SetContent(p_swap_chain1.Get());
    m_p_target->SetRoot(m_p_visual.Get());
}

void CD2D1DeviceContextWindowSupport::CDCompositionHelper::ReinitializeDCompositionPart()
{
    bool topmost = ::GetWindowLong(m_hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;
    ThrowIfFailed<CDCompositionException>(
        m_p_composition_device->CreateTargetForHwnd(
            m_hwnd,
            topmost,
            &m_p_target),
        TRAFFICMONITOR_ERROR_STR("Create dcomposition target for hwnd failed."));

    ThrowIfFailed<CDCompositionException>(
        m_p_composition_device->CreateVisual(&m_p_visual),
        TRAFFICMONITOR_ERROR_STR("Create dcomposition visual failed."));
}

void CD2D1DeviceContextWindowSupport::CDCompositionHelper::ReinitializeD3D10Device1Part()
{
    InitializeSwapChain(m_size);
}

CD2D1DeviceContextWindowSupport::CDCompositionHelper::CDCompositionHelper(CDCompositionDevice& ref_dcomposition_device, CD3D10Device1& ref_d3d10_device1, HWND hwnd)
    : CDCompositionResourceBase{ref_dcomposition_device.GetStorage()}, CD3D10Device1ResourceBase{ref_d3d10_device1.GetStorage()},
      m_p_composition_device{ref_dcomposition_device.Get()}, m_p_d3d10_device1{ref_d3d10_device1.Get()}, m_hwnd{hwnd}
{
    ReinitializeDCompositionPart();
}

void CD2D1DeviceContextWindowSupport::CDCompositionHelper::OnDeviceRecreate(CDCompositionDeviceType p_new_composition_device) noexcept
{
    m_p_composition_device = p_new_composition_device;
    m_need_recreate_dcomposition_device = true;
}

void CD2D1DeviceContextWindowSupport::CDCompositionHelper::OnDeviceRecreate(CD3D10Device1Type p_new_d3d10_device1) noexcept
{
    m_p_d3d10_device1 = p_new_d3d10_device1;
    m_need_recreate_d3d10_device1 = true;
}

bool CD2D1DeviceContextWindowSupport::CDCompositionHelper::HandleDeviceRecreationIfNecessary()
{
    auto result = false;
    if (m_need_recreate_dcomposition_device)
    {
        ReinitializeDCompositionPart();
        result |= std::exchange(m_need_recreate_dcomposition_device, false);
    }
    if (m_need_recreate_d3d10_device1)
    {
        ReinitializeD3D10Device1Part();
        result |= std::exchange(m_need_recreate_d3d10_device1, false);
    }
    return result;
}

auto CD2D1DeviceContextWindowSupport::CDCompositionHelper::Resize(const D2D1_SIZE_U size)
    -> Microsoft::WRL::ComPtr<IDXGISurface>
{
    if (!m_p_swap_chain1 || m_size.height == 0 || m_size.width == 0)
    {
        InitializeSwapChain(size);
    }
    else
    {
        ThrowIfFailed<CDxgiException>(
            m_p_swap_chain1->ResizeBuffers(
                0,
                size.width,
                size.height,
                DXGI_FORMAT_UNKNOWN,
                0),
            TRAFFICMONITOR_ERROR_STR("Resize composition swap chain failed."));
    }

    RebindSwapChain(m_p_swap_chain1);

    ComPtr<IDXGISurface> result{};
    ThrowIfFailed<CDxgiException>(
        m_p_swap_chain1->GetBuffer(
            0,
            IID_PPV_ARGS(&result)),
        TRAFFICMONITOR_ERROR_STR("Get buffer from composition swap chain failed."));
    m_size = size;
    return result;
}

void CD2D1DeviceContextWindowSupport::CDCompositionHelper::Present()
{
    if (m_need_recreate_d3d10_device1 || m_need_recreate_dcomposition_device)
    {
        return;
    }
    ThrowIfFailed<CDxgiException>(
        m_p_swap_chain1->Present(1, 0),
        TRAFFICMONITOR_ERROR_STR("Call IDXGISwapChain::Present(1, 0) failed."));
    ThrowIfFailed<CDCompositionException>(
        m_p_composition_device->Commit(),
        TRAFFICMONITOR_ERROR_STR("Commit dcomposition draw call failed."));
}

auto CD2D1DeviceContextWindowSupport::CDCompositionHelper::GetD3D10Device1()
    -> Microsoft::WRL::ComPtr<ID3D10Device1>
{
    return m_p_d3d10_device1;
}

bool CD2D1DeviceContextWindowSupport::CDCompositionHelper::IsInvalid() const
{
    BOOL result{FALSE};
    ThrowIfFailed<CDCompositionException>(
        m_p_composition_device->CheckDeviceState(&result),
        TRAFFICMONITOR_ERROR_STR("Check dcomposition device state failed."));
    return result == FALSE;
}

auto CD2D1DeviceContextWindowSupport::CD3DHelper::CreateGdiCompatibleTexture(ComPtr<ID3D10Device1> p_device1, const D2D1_SIZE_U size)
    -> Microsoft::WRL::ComPtr<ID3D10Texture2D>
{
    D3D10_TEXTURE2D_DESC description = {};
    description.ArraySize = 1;
    description.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
    description.Format = CTaskBarDlgDrawCommonWindowSupport::PIXEL_FORMAT;
    description.Width = size.width;
    description.Height = size.height;
    description.MipLevels = 1;
    description.SampleDesc.Count = 1;
    description.MiscFlags = D3D10_RESOURCE_MISC_GDI_COMPATIBLE;

    Microsoft::WRL::ComPtr<ID3D10Texture2D> result{};
    ThrowIfFailed<CD3D10Exception1>(
        p_device1->CreateTexture2D(
            &description,
            NULL,
            &result),
        TRAFFICMONITOR_ERROR_STR("CD2D1DeviceContextWindowSupport create texture2D failed."));

    return result;
}

CD2D1DeviceContextWindowSupport::CD3DHelper::CD3DHelper(CD3D10Device1& ref_device)
    : Base{ref_device.GetStorage()}, m_p_device1{ref_device.Get()}
{
}

auto CD2D1DeviceContextWindowSupport::CD3DHelper::Resize(const D2D1_SIZE_U size)
    -> Microsoft::WRL::ComPtr<IDXGISurface>
{
    m_p_render_target_texture = CreateGdiCompatibleTexture(m_p_device1, size);
    ThrowIfFailed<CD3D10Exception1>(
        m_p_render_target_texture.As(&m_p_render_target_surface),
        TRAFFICMONITOR_ERROR_STR("Get IDXGISurface1 from D2D render target texture failed."));
    return m_p_render_target_surface;
}

bool CD2D1DeviceContextWindowSupport::CD3DHelper::HandleDeviceRecreationIfNecessary()
{
    if (m_need_recreate)
    {
        return std::exchange(m_need_recreate, false);
    }
    return true;
}

void CD2D1DeviceContextWindowSupport::CD3DHelper::OnDeviceRecreate(DeviceType p_new_device) noexcept
{
    m_p_device1 = p_new_device;
    m_need_recreate = true;
}

auto CD2D1DeviceContextWindowSupport::CD3DHelper::GetRenderTargetSurface()
    -> Microsoft::WRL::ComPtr<IDXGISurface1>
{
    return m_p_render_target_surface;
}

auto CD2D1DeviceContextWindowSupport::CD3DHelper::GetD3D10Device1()
    -> Microsoft::WRL::ComPtr<ID3D10Device1>
{
    return m_p_device1;
}

auto CD2D1DeviceContextWindowSupport::GetNullableDCompositionHelper()
    -> CNullable<CDCompositionHelper>&
{
    return GetStorageAs<CNullable<CDCompositionHelper>>();
}

auto CD2D1DeviceContextWindowSupport::GetNullableD3DHelper()
    -> CNullable<CD3DHelper>&
{
    return GetStorageAs<CNullable<CD3DHelper>>();
}

void CD2D1DeviceContextWindowSupport::DestroyStorage()
{
    m_size = {};
    Dispatch(
        {{DrawCommonHelper::RenderType::D2D1_WITH_DCOMPOSITION,
          [this]()
          { Destroy(&GetNullableDCompositionHelper()); }},
         {DrawCommonHelper::RenderType::D2D1,
          [this]()
          { Destroy(&GetNullableD3DHelper()); }}});
}

CD2D1DeviceContextWindowSupport::CD2D1DeviceContextWindowSupport(CTaskBarDlgDrawCommonSupport& ref_task_bar_dlg_draw_common_support)
    : m_ref_task_bar_dlg_draw_common_support{ref_task_bar_dlg_draw_common_support},
      m_d2d1_device_context_helper{ref_task_bar_dlg_draw_common_support.GetD2D1Device()}
{
    EmplaceAt(&GetNullableDCompositionHelper());
}

CD2D1DeviceContextWindowSupport::~CD2D1DeviceContextWindowSupport()
{
    DestroyStorage();
}

auto CD2D1DeviceContextWindowSupport::SetWorkingDevice(CDCompositionDevice& ref_dcomposition_device, CD3D10Device1& ref_d3d10_device1, HWND hwnd)
    -> CDCompositionHelper&
{
    auto& ref_nullable_dcomposition_helper = GetNullableDCompositionHelper();
    if (m_render_type == DrawCommonHelper::RenderType::D2D1_WITH_DCOMPOSITION && m_is_initialized)
    {
        auto& result = ref_nullable_dcomposition_helper.GetUnsafe();
        if (result.IsInvalid())
        {
            throw CDCompositionException(
                S_FALSE,
                TRAFFICMONITOR_ERROR_STR("DComposition device is invalid.\nNote: The HRESULT value is not set by Direct Composition API. Please ignore it."));
        }
        return result;
    }
    DestroyStorage();
    EmplaceAt(&ref_nullable_dcomposition_helper);
    m_render_type = DrawCommonHelper::RenderType::D2D1_WITH_DCOMPOSITION;
    ref_nullable_dcomposition_helper.Construct(ref_dcomposition_device, ref_d3d10_device1, hwnd);
    m_is_initialized = true;
    return ref_nullable_dcomposition_helper.GetUnsafe();
}

auto CD2D1DeviceContextWindowSupport::SetWorkingDevice(CD3D10Device1& ref_device1)
    -> CD3DHelper&
{
    auto& ref_nullable_d3d_helper = GetNullableD3DHelper();

    if (m_render_type == DrawCommonHelper::RenderType::D2D1 && m_is_initialized)
    {
        return ref_nullable_d3d_helper.GetUnsafe();
    }
    DestroyStorage();
    EmplaceAt(&ref_nullable_d3d_helper);
    m_render_type = DrawCommonHelper::RenderType::D2D1;
    ref_nullable_d3d_helper.Construct(ref_device1);
    m_is_initialized = true;
    return ref_nullable_d3d_helper.GetUnsafe();
}

auto CD2D1DeviceContextWindowSupport::Resize(const D2D1_SIZE_U size)
    -> Microsoft::WRL::ComPtr<ID2D1DeviceContext>
{
    auto force_refresh = m_d2d1_device_context_helper.HandleDeviceRecreationIfNecessary();
    auto nullable_force_refresh = Dispatch<bool>(
        {{DrawCommonHelper::RenderType::D2D1_WITH_DCOMPOSITION,
          [this]()
          { return GetNullableDCompositionHelper().GetUnsafe().HandleDeviceRecreationIfNecessary(); }},
         {DrawCommonHelper::RenderType::D2D1,
          [this]()
          { return GetNullableD3DHelper().GetUnsafe().HandleDeviceRecreationIfNecessary(); }}});
    force_refresh |= nullable_force_refresh.GetUnsafe();

    if (m_size.width != size.width || m_size.height != size.height || force_refresh)
    {
        m_d2d1_device_context_helper.Reset();
        auto nullable_p_render_target_surface =
            Dispatch<Microsoft::WRL::ComPtr<IDXGISurface>>(
                {{DrawCommonHelper::RenderType::D2D1_WITH_DCOMPOSITION,
                  [this, size]()
                  { return GetNullableDCompositionHelper().GetUnsafe().Resize(size); }},
                 {DrawCommonHelper::RenderType::D2D1,
                  [this, size]()
                  { return GetNullableD3DHelper().GetUnsafe().Resize(size); }}});
        m_d2d1_device_context_helper.RebindRenderTargetSurface(nullable_p_render_target_surface.GetUnsafe());
        m_size = size;
    }
    return m_d2d1_device_context_helper.GetDeviceContext();
}

void CD2D1DeviceContextWindowSupport::PresentWhenUseDComposition()
{
    Dispatch({{DrawCommonHelper::RenderType::D2D1_WITH_DCOMPOSITION,
               [this]()
               { GetNullableDCompositionHelper().GetUnsafe().Present(); }}});
}

auto CD2D1DeviceContextWindowSupport::GetRenderTargetSurfaceWhenUseDefault()
    -> Microsoft::WRL::ComPtr<IDXGISurface1>
{
    if (m_render_type == DrawCommonHelper::RenderType::D2D1)
    {
        return GetNullableD3DHelper().GetUnsafe().GetRenderTargetSurface();
    }
    return {};
}

void CD2D1DeviceContextWindowSupport::RebindD2D1BitmapCache(std::weak_ptr<class CD2D1BitmapCache> wp_cache)
{
    m_d2d1_device_context_helper.RebindD2D1BitmapCache(wp_cache);
}

void CD2D1DeviceContextWindowSupport::SetBackColor(COLORREF color, BYTE alpha) noexcept
{
    m_d2d1_device_context_helper.SetBackColor(color, alpha);
}

auto CD2D1DeviceContextWindowSupport::GetRawBackSolidColorBruch() noexcept
    -> ID2D1SolidColorBrush*
{
    return m_d2d1_device_context_helper.GetRawBackSolidColorBruch();
}

auto CD2D1DeviceContextWindowSupport::SetForeColor(COLORREF color, BYTE alpha) noexcept
    -> CD2D1DeviceContextWindowSupport*
{
    m_d2d1_device_context_helper.SetForeColor(color, alpha);
    return this;
}

auto CD2D1DeviceContextWindowSupport::GetRawForeSolidColorBruch() noexcept
    -> ID2D1SolidColorBrush*
{
    return m_d2d1_device_context_helper.GetRawForeSolidColorBruch();
}

auto CD2D1DeviceContextWindowSupport::GetCachedBitmap(HBITMAP hbitmap)
    -> Microsoft::WRL::ComPtr<ID2D1Bitmap>
{
    return m_d2d1_device_context_helper.GetCachedBitmap(hbitmap);
}

void CD2D1DeviceContextWindowSupport::RequestD3D10Device1Recreate()
{
    auto nullable_p_d3d10_device1 = Dispatch<Microsoft::WRL::ComPtr<ID3D10Device1>>(
        {{DrawCommonHelper::RenderType::D2D1_WITH_DCOMPOSITION,
          [this]()
          { return GetNullableDCompositionHelper().GetUnsafe().GetD3D10Device1(); }},
         {DrawCommonHelper::RenderType::D2D1,
          [this]()
          { return GetNullableD3DHelper().GetUnsafe().GetD3D10Device1(); }}});
    auto p_d3d10_device1 = nullable_p_d3d10_device1.GetUnsafe();
    auto recreate_reason = p_d3d10_device1->GetDeviceRemovedReason();
    m_ref_task_bar_dlg_draw_common_support.RecreateD3D10Device1(recreate_reason);
}

void CD2D1DeviceContextWindowSupport::RequestD2D1DeviceRecreate(const HRESULT recreate_reason)
{
    m_ref_task_bar_dlg_draw_common_support.RecreateD2D1Device(recreate_reason);
}

void CD2D1DeviceContextWindowSupport::RequestDCompositionDeviceRecreate(const HRESULT recreate_reason)
{
    m_ref_task_bar_dlg_draw_common_support.RecreateDCompositionDevice(recreate_reason);
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
        TRAFFICMONITOR_ERROR_STR("Call IWICImagingFactory::CreateBitmapFromHBITMAP failed."));

    ComPtr<ID2D1Bitmap> result{};
    ThrowIfFailed<CD2D1Exception>(
        p_render_target->CreateBitmapFromWicBitmap(
            p_wic_bitmap.Get(),
            &result),
        TRAFFICMONITOR_ERROR_STR("Call ID2D1RenderTarget::CreateBitmapFromWicBitmap failed."));

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

void CTaskBarDlgDrawCommon::ResetClippedStateIfSet()
{
    if (m_is_clipped)
    {
        m_p_device_context->PopAxisAlignedClip();
    }
}

void CTaskBarDlgDrawCommon::Create(
    CTaskBarDlgDrawCommonWindowSupport& taskbar_dlg_draw_common_window_support,
    CD2D1DeviceContextWindowSupport& ref_d2d1_device_window_support,
    const D2D1_SIZE_U size)
{
    const static auto transparent_black =
        CRenderTarget::COLORREF_TO_D2DCOLOR(RGB(0, 0, 0), 0);
    const static auto gdi_back_color =
        CRenderTarget::COLORREF_TO_D2DCOLOR(RGB(0, 0, 0), DEFAULT_GDI_OP_TEXTURE_ALPHA);

    m_p_window_support = &taskbar_dlg_draw_common_window_support;
    m_p_window_support->Resize(size);
    m_p_d2d1_device_context_support = &ref_d2d1_device_window_support;
    m_p_device_context = m_p_d2d1_device_context_support->Resize(size);

    m_p_device_context->BeginDraw();
    m_p_device_context->SetTransform(D2D1::Matrix3x2F::Identity());
    m_p_device_context->Clear(transparent_black);
}

auto CTaskBarDlgDrawCommon::GetD3D10Device1RecreateRequester()
    -> std::function<bool(CHResultException& ex)>
{
    return [p_window_support = m_p_window_support](CHResultException& ex)
    {
        return DrawCommonHelper::HandleIfNeedRecreate(
            ex,
            [p_window_support]()
            { p_window_support->RequestD3D10Device1Recreate(); });
    };
}

auto CTaskBarDlgDrawCommon::GetD2D1DeviceRecreateRequester()
    -> std::function<bool(CHResultException& ex)>
{
    return [p_d2d1_device_context_support = m_p_d2d1_device_context_support](CHResultException& ex)
    {
        return DrawCommonHelper::HandleIfD2D1DeviceNeedRecreate(
            ex,
            [&]()
            { p_d2d1_device_context_support->RequestD2D1DeviceRecreate(ex.GetHResult()); });
    };
}

auto CTaskBarDlgDrawCommon::GetDCompositionDeviceRecreateRequester()
    -> std::function<bool(CHResultException& ex)>
{
    return [p_d2d1_device_context_support = m_p_d2d1_device_context_support](CHResultException& ex)
    {
        return DrawCommonHelper::HandleIfNeedRecreate(
            ex,
            [&]()
            { p_d2d1_device_context_support->RequestDCompositionDeviceRecreate(ex.GetHResult()); });
    };
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
    try
    {
        if (m_gdi_interop_object.HasValue())
        {
            auto texture_size = m_p_window_support->GetSize();
            auto data_size = texture_size.height * texture_size.width * 4;
            auto& ref_gdi_interop_object = m_gdi_interop_object.Get();
            m_p_window_support->ResizeGdiInteropPart();
            ::GdiFlush();
            m_p_window_support->SetGdiInteropTexture(
                ref_gdi_interop_object.m_p_gdi_interop_hbitmap_data, texture_size);
            auto waiter = m_p_window_support->DrawAlphaValueReduceEffect();

            ThrowIfFailed<CD2D1Exception>(
                m_p_device_context->Flush(),
                TRAFFICMONITOR_ERROR_STR("Call ID2D1RenderTarget::Flush() failed."));

            ThrowIfFailed<CD3D10Exception1>(
                waiter.Wait(),
                TRAFFICMONITOR_ERROR_STR("Draw call failed."));

            D2D1_BITMAP_PROPERTIES d2d1_bitmap_properties{};
            d2d1_bitmap_properties.pixelFormat.format = CTaskBarDlgDrawCommonWindowSupport::PIXEL_FORMAT;
            d2d1_bitmap_properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
            d2d1_bitmap_properties.dpiX = CTaskBarDlgDrawCommonWindowSupport::DEFAULT_DPI;
            d2d1_bitmap_properties.dpiY = CTaskBarDlgDrawCommonWindowSupport::DEFAULT_DPI;
            auto p_gdi_interop_surface1 = m_p_window_support->GetGdiFinalTextureSurface();
            ComPtr<ID2D1Bitmap> p_gdi_interop_bitmap{};
            ThrowIfFailed<CD2D1Exception>(
                m_p_device_context->CreateSharedBitmap(
                    __uuidof(IDXGISurface1),
                    p_gdi_interop_surface1.Get(),
                    &d2d1_bitmap_properties,
                    &p_gdi_interop_bitmap),
                TRAFFICMONITOR_ERROR_STR("Create ID2D1Bitmap from IDXGISurface1 failed."));
            m_p_device_context->DrawBitmap(p_gdi_interop_bitmap.Get());
        }
        // ! 注意：保证正常时EndDraw在不起用插件时仍然可以被调用，否则会导致D2D Begin/End调用不匹配，已经是第二次出错了。
        ThrowIfFailed<CD2D1Exception>(
            m_p_device_context->EndDraw(),
            TRAFFICMONITOR_ERROR_STR("Call ID2D1RenderTarget::EndDraw() failed."));
    }
    catch (CHResultException& ex)
    {
        LogHResultException(ex);
    }
}

void CTaskBarDlgDrawCommon::SetBackColor(COLORREF back_color, BYTE alpha)
{
    m_p_d2d1_device_context_support->SetBackColor(back_color, alpha);
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
        CDWriteSupport::GetFactory()->CreateTextLayout(
            lpszString,
            length_u,
            p_text_format.Get(),
            layout_rect.right - layout_rect.left,
            layout_rect.bottom - layout_rect.top,
            &p_text_layout),
        TRAFFICMONITOR_ERROR_STR("Function CreateTextLayout failed."));
    DWRITE_OVERHANG_METRICS delta_size;
    p_text_layout->GetOverhangMetrics(&delta_size);
    D2D1_RECT_F rect_f{
        delta_size.left > 0 ? delta_size.left + layout_rect.left : layout_rect.left,
        delta_size.top > 0 ? delta_size.top + layout_rect.top : layout_rect.top,
        delta_size.right > 0 ? delta_size.right + layout_rect.right : layout_rect.right,
        delta_size.bottom > 0 ? delta_size.bottom + layout_rect.bottom : layout_rect.bottom};
    if (draw_back_ground)
    {
        m_p_device_context->FillRectangle(rect_f, m_p_d2d1_device_context_support->GetRawBackSolidColorBruch());
    }

    m_p_d2d1_device_context_support->SetForeColor(color, alpha);
    D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NO_SNAP | D2D1_DRAW_TEXT_OPTIONS_CLIP; // 不允许文字超出边界
    if (theApp.m_taskbar_data.enable_colorful_emoji)
        options |= D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT;
    m_p_device_context->DrawTextLayout(
        {layout_rect.left, layout_rect.top},
        p_text_layout.Get(),
        m_p_d2d1_device_context_support->GetRawForeSolidColorBruch(),
        options);
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
    m_p_device_context->PushAxisAlignedClip(d2d1_clip_rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    m_is_clipped = true;
}

void CTaskBarDlgDrawCommon::FillRect(CRect rect, COLORREF color, BYTE alpha)
{
    auto rect_f = Convert(rect);
    m_p_d2d1_device_context_support->SetForeColor(color, alpha);
    m_p_device_context->FillRectangle(rect_f, m_p_d2d1_device_context_support->GetRawForeSolidColorBruch());
}

void CTaskBarDlgDrawCommon::DrawRectOutLine(CRect rect, COLORREF color, int width, bool dot_line, BYTE alpha)
{
    rect.DeflateRect(width / 2, width / 2);
    auto rect_f = Convert(rect);
    m_p_d2d1_device_context_support->SetForeColor(color, alpha);
    auto width_f = static_cast<float>(width);
    if (dot_line)
    {
        m_p_device_context
            ->DrawRectangle(
                rect_f,
                m_p_d2d1_device_context_support->GetRawForeSolidColorBruch(),
                width_f,
                m_p_window_support->GetRawPsDotLikeStyle());
    }
    else
    {
        m_p_device_context
            ->DrawRectangle(
                rect_f,
                m_p_d2d1_device_context_support->GetRawForeSolidColorBruch(),
                width_f);
    }
}

void CTaskBarDlgDrawCommon::DrawLine(CPoint start_point, int height, COLORREF color, BYTE alpha)
{
    auto d2d1_start_point = Convert(start_point);
    D2D1_POINT_2F d2d1_end_point{d2d1_start_point.x, d2d1_start_point.y - height};
    m_p_d2d1_device_context_support->SetForeColor(color, alpha);
    m_p_device_context->DrawLine(
        d2d1_start_point,
        d2d1_end_point,
        m_p_d2d1_device_context_support->GetRawForeSolidColorBruch());
}

void CTaskBarDlgDrawCommon::SetTextColor(const COLORREF color, BYTE alpha)
{
    m_text_color = color;
    m_p_d2d1_device_context_support->SetForeColor(color, alpha);
}

void CTaskBarDlgDrawCommon::DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode, BYTE alpha)
{
    auto p_d2d1_bitmap = m_p_d2d1_device_context_support->GetCachedBitmap(hbitmap);
    if (!p_d2d1_bitmap)
    {
        CD2D1BitmapCache::CreateD2D1BitmapFromHBitmap(m_p_device_context, hbitmap);
    }

    float opacity = static_cast<float>(alpha) / 255.f;
    D2D1_RECT_F draw_rect_f;
    if (size.cx == 0 || size.cy == 0)
    {
        auto bitmap_size = p_d2d1_bitmap->GetSize();
        draw_rect_f.left = static_cast<float>(start_point.x);
        draw_rect_f.top = static_cast<float>(start_point.y);
        draw_rect_f.right = static_cast<float>(start_point.x) + bitmap_size.width;
        draw_rect_f.bottom = static_cast<float>(start_point.y) + bitmap_size.height;
        m_p_device_context->DrawBitmap(p_d2d1_bitmap.Get(), draw_rect_f, opacity);
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
        m_p_device_context->DrawBitmap(p_d2d1_bitmap.Get(), draw_rect_f, opacity);
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
    m_p_device_context->DrawBitmap(p_d2d1_bitmap.Get(), draw_rect_f, opacity);
}

CTaskBarDlgDrawBuffer::CTaskBarDlgDrawBuffer(
    CTaskBarDlgDrawCommonWindowSupport& taskbar_dlg_draw_common_window_support,
    CD2D1DeviceContextWindowSupport& ref_d2d1_device_context_support,
    CSize size,
    HWND hwnd)
    : m_p_gdi_surface{ref_d2d1_device_context_support.GetRenderTargetSurfaceWhenUseDefault()},
      m_size{size}, m_target_hwnd{hwnd}, m_ref_window_support{taskbar_dlg_draw_common_window_support}
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
    try
    {
        ThrowIfFailed<CD3D10Exception1>(
            m_p_gdi_surface->GetDC(FALSE, &gdi_dc),
            TRAFFICMONITOR_ERROR_STR("Get DC from IDXGISurface1 failed."));

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
        auto state = ::UpdateLayeredWindowIndirect(m_target_hwnd, &update_window_info);
        if (state == 0)
        {
            auto error_code = ::GetLastError();
            theApp.m_taskbar_data.update_layered_window_error_code = error_code;
        }
        RECT empty_rect{};
        m_p_gdi_surface->ReleaseDC(&empty_rect);
        DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler::ResetErrorCount();
    }
    catch (CD3D10Exception1& ex)
    {
        LogHResultException(ex);
    }
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

CTaskBarDlgDrawBufferUseDComposition::CTaskBarDlgDrawBufferUseDComposition(CD2D1DeviceContextWindowSupport& ref_d2d1_device_context_support)
{
    m_p_d2d1_device_context_support = &ref_d2d1_device_context_support;
}

CTaskBarDlgDrawBufferUseDComposition::~CTaskBarDlgDrawBufferUseDComposition()
{
    if (!m_p_d2d1_device_context_support)
    {
        return;
    }
    try
    {
        m_p_d2d1_device_context_support->PresentWhenUseDComposition();
        DrawCommonHelper::DefaultD2DDrawCommonExceptionHandler::ResetErrorCount();
    }
    catch (CHResultException& ex)
    {
        LogHResultException(ex);
    }
}
