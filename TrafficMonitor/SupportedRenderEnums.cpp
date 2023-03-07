#include "stdafx.h"
#include "SupportedRenderEnums.h"
#include "TaskBarDlgDrawCommon.h"

CSupportedRenderEnums::CSupportedRenderEnums()
{
    if (CDCompositionSupport::CheckSupport() && CDxgi1Support2::CheckSupport())
    {
        m_enums[D2D1_WITH_DCOMPOSITION_INDEX] = true;
    }
    if (CTaskBarDlgDrawCommonSupport::CheckSupport())
    {
        m_enums[D2D1_INDEX] = true;
    }
}

void CSupportedRenderEnums::EnableDefaultOnly() noexcept
{
    m_enums = decltype(m_enums){1};
}

void CSupportedRenderEnums::DisableD2D1() noexcept
{
    m_enums[D2D1_INDEX] = false;
}

auto CSupportedRenderEnums::GetAutoFitEnum() const noexcept
    -> DrawCommonHelper::RenderType
{
    std::size_t i;
    for (i = m_enums.size() - 1; i > 0; --i)
    {
        if (m_enums[i])
        {
            break;
        }
    }
    switch (i)
    {
    case D2D1_INDEX:
        return DrawCommonHelper::RenderType::D2D1;
    case D2D1_WITH_DCOMPOSITION_INDEX:
        return DrawCommonHelper::RenderType::D2D1_WITH_DCOMPOSITION;
    case DEFAULT_INDEX:
        return DrawCommonHelper::RenderType::DEFAULT;
    default:
        return DrawCommonHelper::RenderType::DEFAULT;
    }
}

auto CSupportedRenderEnums::GetMaxSupportedRenderEnum() const noexcept
    -> DrawCommonHelper::RenderType
{
    if (m_enums[D2D1_WITH_DCOMPOSITION_INDEX])
    {
        return DrawCommonHelper::RenderType::D2D1_WITH_DCOMPOSITION;
    }
    else if (m_enums[D2D1_INDEX])
    {
        return DrawCommonHelper::RenderType::D2D1;
    }
    else if (m_enums[DEFAULT_INDEX])
    {
        return DrawCommonHelper::RenderType::DEFAULT;
    }
    return DrawCommonHelper::RenderType::DEFAULT;
}

bool CSupportedRenderEnums::IsD2D1WithDCompositionEnabled() const noexcept
{
    return m_enums[D2D1_WITH_DCOMPOSITION_INDEX];
}

bool CSupportedRenderEnums::IsD2D1Enabled() const noexcept
{
    return m_enums[D2D1_INDEX];
}