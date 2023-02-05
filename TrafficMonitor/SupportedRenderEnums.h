#pragma once
#include <bitset>
#include "IDrawCommon.h"

class CSupportedRenderEnums
{
private:
    std::bitset<3> m_enums{1};

public:
    constexpr static std::size_t DEFAULT_INDEX = 0;
    constexpr static std::size_t D2D1_WITH_DCOMPOSITION_INDEX = 1;
    constexpr static std::size_t D2D1_INDEX = 2;

    CSupportedRenderEnums();
    ~CSupportedRenderEnums() = default;
    void EnableDefaultOnly() noexcept;
    void DisableD2D1() noexcept;
    /**
     * @brief 根据D2D1 > D2D1_WITH_DCOMPOSITION > DEFAULT的顺序返回枚举
     *
     * @return DrawCommonHelper::RenderType
     */
    auto GetAutoFitEnum() const noexcept
        -> DrawCommonHelper::RenderType;
    /**
     * @brief （已弃用）
        根据D2D1_WITH_DCOMPOSITION > D2D1 > DEFAULT的顺序返回枚举
     *
     * @return DrawCommonHelper::RenderType
     */
    [[deprecated("May no longer be needed.")]]
    auto GetMaxSupportedRenderEnum() const noexcept
        -> DrawCommonHelper::RenderType;
    bool IsD2D1WithDCompositionEnabled() const noexcept;
    bool IsD2D1Enabled() const noexcept;
};
