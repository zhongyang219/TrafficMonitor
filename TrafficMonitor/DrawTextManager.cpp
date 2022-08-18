#include "stdafx.h"
#include "DrawTextManager.h"
#include <memoryapi.h>

EnableWriteMemoryGuard::~EnableWriteMemoryGuard()
{
    m_state = ::VirtualProtect(m_p_memory, m_memory_size, m_last_flag, &m_last_flag);
}

bool EnableWriteMemoryGuard::GetState()
{
    return m_state;
}

int WINAPI User32DrawTextManager::A::CustomDrawTextA(HDC hdc, LPCSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    if (BaseSettings::GetEnable())
    {
        return BaseSettings::m_replaced_function(hdc, lpchText, cchText, lprc, format);
    }
    else
    {
        return (BaseSettings::GetOriginalFunction())(hdc, lpchText, cchText, lprc, format);
    }
}

auto User32DrawTextManager::A::GetFunction() noexcept
    -> Function
{
    return &CustomDrawTextA;
}

int WINAPI User32DrawTextManager::W::CustomDrawTextW(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    if (BaseSettings::GetEnable())
    {
        return BaseSettings::m_replaced_function(hdc, lpchText, cchText, lprc, format);
    }
    else
    {
        return (BaseSettings::GetOriginalFunction())(hdc, lpchText, cchText, lprc, format);
    }
}

auto User32DrawTextManager::W::GetFunction() noexcept
    -> Function
{
    return &CustomDrawTextW;
}

int WINAPI User32DrawTextManager::ExA::CustomDrawTextExA(HDC hdc, LPSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp)
{
    if (BaseSettings::GetEnable())
    {
        return BaseSettings::m_replaced_function(hdc, lpchText, cchText, lprc, format, lpdtp);
    }
    else
    {
        return (BaseSettings::GetOriginalFunction())(hdc, lpchText, cchText, lprc, format, lpdtp);
    }
}

auto User32DrawTextManager::ExA::GetFunction() noexcept
    -> Function
{
    return &CustomDrawTextExA;
}

int WINAPI User32DrawTextManager::ExW::CustomDrawTextExW(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp)
{
    if (BaseSettings::GetEnable())
    {
        return BaseSettings::m_replaced_function(hdc, lpchText, cchText, lprc, format, lpdtp);
    }
    else
    {
        return (BaseSettings::GetOriginalFunction())(hdc, lpchText, cchText, lprc, format, lpdtp);
    }
}

auto User32DrawTextManager::ExW::GetFunction() noexcept
    -> Function
{
    return &CustomDrawTextExW;
}
