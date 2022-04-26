#include "stdafx.h"
#include "D2D1Support.h"

#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "Dwrite.lib")

namespace FunctionChecker
{
    namespace Details
    {
        template <class Strategy>
        bool StrategicCheck(LPCTSTR p_library_name, Strategy strategy)
        {
            bool result = false;
            auto hmodule = ::LoadLibrary(p_library_name);
            if (hmodule)
            {
                result = strategy(hmodule);
                ::FreeLibrary(hmodule);
            }
            return result;
        }
    }
} // namespace FunctionChecker

bool FunctionChecker::CheckLibraryExist(LPCTSTR p_library_name) noexcept
{
    return Details::StrategicCheck(p_library_name,
                                   [](HMODULE)
                                   { return true; });
}

bool FunctionChecker::CheckFunctionExist(LPCTSTR p_library_name, LPCSTR p_function_name) noexcept
{
    return Details::StrategicCheck(p_library_name,
                                   [p_function_name](HMODULE h_library) -> bool
                                   {
                                       auto* p_test_function = ::GetProcAddress(h_library, p_function_name);
                                       return p_test_function == NULL;
                                   });
}

void ThrowIfFailed(HRESULT hr, const char* p_message)
{
    if (FAILED(hr))
    {
        throw HResultException{hr, p_message};
    }
}

HResultException::HResultException(HRESULT hr, const char* p_message)
    : std::runtime_error{p_message}
{
    m_hr = ::GetErrorInfo(0, &m_p_error);
}

auto HResultException::GetError()
    -> Microsoft::WRL::ComPtr<IErrorInfo>
{
    return m_p_error;
}

bool HResultException::HasError() const noexcept
{
    return SUCCEEDED(m_hr);
}

bool D2D1Support::CheckSupport()
{
    static bool result = FunctionChecker::CheckFunctionExist(_T("D2d1.dll"), "D2D1CreateFactory");
    return result;
}

ID2D1Factory* D2D1Support::GetFactory()
{
    static auto result = MakeStaticVariableWrapper<ID2D1Factory*>(
        [](ID2D1Factory** pp_factory)
        {
            *pp_factory = nullptr;
            D2D1_FACTORY_OPTIONS d2d1_factory_options{D2D1_DEBUG_LEVEL_ERROR};
            ThrowIfFailed(::D2D1CreateFactory(
                              D2D1_FACTORY_TYPE_MULTI_THREADED,
                              d2d1_factory_options,
                              pp_factory),
                          "Create D2D1 factory failed.");
        },
        [](ID2D1Factory** pp_factory)
        {
            auto* p_factory = *pp_factory;
            SAFE_RELEASE(p_factory);
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
            SAFE_RELEASE(p_factory);
        });
    return result.Get();
}

D2D1DCSupport::D2D1DCSupport()
{
    D2D1_RENDER_TARGET_PROPERTIES d2d1_reder_target_properties{
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(
            DXGI_FORMAT_B8G8R8A8_UNORM,
            D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0,
        D2D1_RENDER_TARGET_USAGE_NONE,
        D2D1_FEATURE_LEVEL_DEFAULT};
    ThrowIfFailed(D2D1Support::GetFactory()->CreateDCRenderTarget(&d2d1_reder_target_properties, &m_p_render_target),
                  "Create d2d1 dc render target failed.");
};

auto D2D1DCSupport::GetRenderTarget() const
    -> Microsoft::WRL::ComPtr<ID2D1DCRenderTarget>
{
    return m_p_render_target;
}