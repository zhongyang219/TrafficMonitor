#pragma once
#include <shellscalingapi.h> // 包含::GetDpiForMonitor
#include <utility>

typedef HRESULT(WINAPI* _GetDpiForMonitor)(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);

class CDllFunctions
{
public:
    CDllFunctions();
    ~CDllFunctions();

public:
    HRESULT GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);

private:
    _GetDpiForMonitor m_getDpiForMonitor{};

private:
    HMODULE m_shcore_module{};
};

template <class FunctionPointer>
class CDllFunction
#ifdef WIN32
{
    // workaround for MSVC 19.34.31933 target x86

private:
    FunctionPointer m_p_function{nullptr};
    HMODULE m_h_dll;

public:
    CDllFunction(LPCTSTR dll_name, LPCSTR function_name) noexcept
    {
        m_h_dll = ::LoadLibrary(dll_name);
        if (m_h_dll != NULL)
        {
            m_p_function = (FunctionPointer)::GetProcAddress(m_h_dll, function_name);
            if (m_p_function == nullptr)
            {
                ::FreeLibrary(m_h_dll);
                m_h_dll = NULL;
            }
        }
    }
    ~CDllFunction() noexcept
    {
        if (m_h_dll != NULL)
        {
            ::FreeLibrary(m_h_dll);
            m_h_dll = NULL;
        }
    }
    template <class... Args>
    auto operator()(Args&&... args) const
    {
        return m_p_function(std::forward<Args>(args)...);
    }
    bool HasValue() const noexcept
    {
        return m_p_function != nullptr;
    }
}
#endif
;
/**
 * @brief 可以自动管理HMODULE生命周期，并自动尝试加载指定函数的类
 使用示例：
    定义：CDllFunction<decltype(&::GetDpiForMonitor)> GetDpiForMonitor{_T("Shcore.dll"), "GetDpiForMonitor"};
    使用：(某个类或者命名空间)::GetDpiForMonitor(所需的参数);
    当然，变量GetDpiForMonitor不能暴露在全局命名空间内。
 *
 * @tparam R 函数返回值
 * @tparam Args 函数参数
 */
template <class R, class... Args>
class CDllFunction<R (*)(Args...)>
{
    using FunctionPointer = R (*)(Args...);

private:
    FunctionPointer m_p_function{nullptr};
    HMODULE m_h_dll;

public:
    CDllFunction(LPCTSTR dll_name, LPCSTR function_name) noexcept
    {
        m_h_dll = ::LoadLibrary(dll_name);
        if (m_h_dll != NULL)
        {
            m_p_function = (FunctionPointer)::GetProcAddress(m_h_dll, function_name);
            if (m_p_function == nullptr)
            {
                ::FreeLibrary(m_h_dll);
                m_h_dll = NULL;
            }
        }
    }
    ~CDllFunction() noexcept
    {
        if (m_h_dll != NULL)
        {
            ::FreeLibrary(m_h_dll);
            m_h_dll = NULL;
        }
    }
    R operator()(Args... args) const
    {
        return m_p_function(args...);
    }
    bool HasValue() const noexcept
    {
        return m_p_function != nullptr;
    }
};

template <class FunctionPointer>
auto MakeDllFunction(FunctionPointer, LPCTSTR dll_name, LPCSTR function_name) noexcept
    -> CDllFunction<FunctionPointer>
{
    return CDllFunction<FunctionPointer>(dll_name, function_name);
}

template <class FunctionPointer, LPCTSTR DllName, LPCSTR FunctionName>
class CLazyInitializeDllFunction
{
    static auto Get()
        -> CDllFunction<FunctionPointer>&
    {
        static const CDllFunction<FunctionPointer> result{DllName, FunctionName};
        return result;
    }
};

#define TRAFFICMONITOR_DEFINE_LAZY_INITIALIZE_DLL_FUNCTION(function_name_to_used, function_pointer, dll_name_literals, function_name_literals) \
    LPCTSTR dll_name##__LINE__##TRAFFICMONITOR_DEFINE_LAZY_INITIALIZE_DLL_FUNCTION##__internal{dll_name_literals};                             \
    LPCSTR function_name##__LINE__##TRAFFICMONITOR_DEFINE_LAZY_INITIALIZE_DLL_FUNCTION##__internal{function_name_literals};                    \
    using function_name_to_used = CLazyInitializeDllFunction<                                                                                  \
        decltype(&function_pointer),                                                                                                           \
        dll_name##__LINE__##TRAFFICMONITOR_DEFINE_LAZY_INITIALIZE_DLL_FUNCTION##__internal,                                                    \
        function_name##__LINE__##TRAFFICMONITOR_DEFINE_LAZY_INITIALIZE_DLL_FUNCTION##__internal>;
