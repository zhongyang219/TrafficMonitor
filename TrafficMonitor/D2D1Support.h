#pragma once
#include <stdexcept>
#include <wrl/client.h> //ComPtr
#include <d2d1.h>
#include <dwrite.h>

#define RELEASE_COM(p)     \
    {                       \
        if (p)              \
        {                   \
            (p)->Release(); \
            (p) = (NULL);   \
        }                   \
    }

namespace FunctionChecker
{
    bool CheckLibraryExist(LPCTSTR p_library_name) noexcept;
    bool CheckFunctionExist(LPCTSTR p_library_name, LPCSTR p_function_name) noexcept;
}

class HResultException : public std::runtime_error
{
public:
    HResultException(HRESULT hr, const char* p_message);
    ~HResultException() override = default;

    auto GetError()
        -> Microsoft::WRL::ComPtr<IErrorInfo>;
    bool HasError() const noexcept;

private:
    Microsoft::WRL::ComPtr<IErrorInfo> m_p_error;
    HRESULT m_hr;
};

class D2D1Exception final : public HResultException
{
    using HResultException::HResultException;
};

class DWriteException final : public HResultException
{
    using HResultException::HResultException;
};

#define D2D1_SUPPORT_DEFAULT_EXPECTION_TIP "Error occurred when call COM function."
void ThrowIfFailed(HRESULT hr, const char* p_message = D2D1_SUPPORT_DEFAULT_EXPECTION_TIP);
template <class Exception>
void ThrowIfFailed(HRESULT hr, const char* p_message = D2D1_SUPPORT_DEFAULT_EXPECTION_TIP)
{
    if (FAILED(hr))
    {
        throw Exception{hr, p_message};
    }
}
#undef D2D1_SUPPORT_DEFAULT_EXPECTION_TIP

void LogHResultException(HResultException& ex);

class D2D1Support
{
public:
    static bool CheckSupport();
    static ID2D1Factory* GetFactory();
};

class D2D1DCSupport
{
public:
    D2D1DCSupport();
    ~D2D1DCSupport();
    static bool CheckSupport();
    auto GetRenderTarget()
        -> Microsoft::WRL::ComPtr<ID2D1DCRenderTarget>;
    auto GetWeakRenderTarget()
        -> ID2D1DCRenderTarget*;
    auto GetWeakSolidColorBrush()
        -> ID2D1SolidColorBrush*;
    auto GetWeakPsDotStyle()
        -> ID2D1StrokeStyle*;
    auto GetWeakDWriteGdiInterop()
        -> IDWriteGdiInterop*;
    auto GetWeakSoildBackColorBrush()
        -> ID2D1SolidColorBrush*;

private:
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> m_p_render_target{NULL};
    ID2D1SolidColorBrush* m_p_soild_color_brush{NULL};
    ID2D1SolidColorBrush* m_p_soild_back_color_brush{NULL};
    ID2D1StrokeStyle* m_p_ps_dot_style{NULL};
    IDWriteGdiInterop* m_p_dwrite_gdi_interop{NULL};
};

class DWriteSupport
{
public:
    static bool CheckSupport();
    static IDWriteFactory* GetFactory();
};
