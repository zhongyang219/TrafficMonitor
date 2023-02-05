#pragma once
#include <stdexcept>
#include <wrl/client.h>

#define TRAFFICMONITOR_STR_IMPL(x) #x
#define TRAFFICMONITOR_STR(x) TRAFFICMONITOR_STR_IMPL(x)

#define TRAFFICMONITOR_ERROR_STR(x) \
    "At file: " TRAFFICMONITOR_STR(__FILE__) " | line: " TRAFFICMONITOR_STR(__LINE__) ".\nError: " x

#define RELEASE_COM(p)      \
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

class CHResultException : public std::runtime_error
{
public:
    CHResultException(HRESULT hr, const char* p_message);
    ~CHResultException() override = default;

    auto GetError()
        -> Microsoft::WRL::ComPtr<IErrorInfo>;
    bool HasError() const noexcept;
    auto GetHResult() const noexcept
        -> HRESULT;

private:
    Microsoft::WRL::ComPtr<IErrorInfo> m_p_error;
    HRESULT m_hr;
    HRESULT m_get_p_error_hr;
};

extern const char* const ERROR_WHEN_CALL_COM_FUNCTION; /*The content should be "Error occurred when call COM function." */
void ThrowIfFailed(HRESULT hr, const char* p_message = ERROR_WHEN_CALL_COM_FUNCTION);
template <class Exception, class... Args>
void ThrowIfFailed(HRESULT hr, const char* p_message, Args... args)
{
    if (FAILED(hr))
    {
        throw Exception{hr, p_message, std::forward<Args>(args)...};
    }
}

void LogHResultException(CHResultException& ex);
