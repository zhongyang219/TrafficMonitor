#pragma once
#include <wrl/client.h>
#include "Common.h"

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
        ->Microsoft::WRL::ComPtr<IErrorInfo>;
    bool HasError() const noexcept;

private:
    Microsoft::WRL::ComPtr<IErrorInfo> m_p_error;
    HRESULT m_hr;
};

extern const char* const ERROR_WHEN_CALL_COM_FUNCTION; /*The content should be "Error occurred when call COM function." */
void ThrowIfFailed(HRESULT hr, const char* p_message = ERROR_WHEN_CALL_COM_FUNCTION);
template <class Exception>
void ThrowIfFailed(HRESULT hr, const char* p_message = ERROR_WHEN_CALL_COM_FUNCTION)
{
    if (FAILED(hr))
    {
        throw Exception{ hr, p_message };
    }
}

void LogHResultException(CHResultException& ex);