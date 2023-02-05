#include "stdafx.h"
#include "HResultException.h"
#include "TrafficMonitor.h"

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
                                       return p_test_function != NULL;
                                   });
}

const char* const ERROR_WHEN_CALL_COM_FUNCTION = "Error occurred when call COM function.";

void ThrowIfFailed(HRESULT hr, const char* p_message)
{
    ThrowIfFailed<CHResultException>(hr, std::move(p_message));
}

CHResultException::CHResultException(HRESULT hr, const char* p_message)
    : std::runtime_error{p_message}, m_hr{hr},
      m_get_p_error_hr{::GetErrorInfo(0, &m_p_error)}
{
}

auto CHResultException::GetError()
    -> Microsoft::WRL::ComPtr<IErrorInfo>
{
    return m_p_error;
}

bool CHResultException::HasError() const noexcept
{
    return SUCCEEDED(m_hr);
}

auto CHResultException::GetHResult() const noexcept
    -> HRESULT
{
    return m_hr;
}

void LogHResultException(CHResultException& ex)
{
    auto str_hr = "HResult:" + std::to_string(ex.GetHResult());
    CCommon::WriteLog(str_hr.c_str(), theApp.m_log_path.c_str());
    auto* log = ex.what();
    CCommon::WriteLog(log, theApp.m_log_path.c_str());
    auto* p_error = ex.GetError().Get();
    if (p_error == NULL)
    {
        return;
    }
    BSTR p_description{NULL};
    ThrowIfFailed(p_error->GetDescription(&p_description),
                  TRAFFICMONITOR_ERROR_STR("Get description from IErrorInfo failed."));
    CCommon::WriteLog(p_description, theApp.m_log_path.c_str());
    ::SysFreeString(p_description);
}