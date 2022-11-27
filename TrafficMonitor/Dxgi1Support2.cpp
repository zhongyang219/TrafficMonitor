#include "stdafx.h"
#include "Dxgi1Support2.h"
#include "Common.h"
#include "DllFunctions.h"

void CDxgiSwapChain1::Recreate(Microsoft::WRL::ComPtr<IUnknown> p_device, const DXGI_SWAP_CHAIN_DESC1& ref_desc1, IDXGIOutput* p_output)
{
    CallFunctionForEachResource<TRAFFICMONITOR_DV_V_ARGS(&Resource::OnSwapChainResizeBegin)>(m_resource_tracker);

    ThrowIfFailed<CDxgiException>(
        CDxgi1Support2::GetFactory()->CreateSwapChainForComposition(
            p_device.Get(),
            &ref_desc1,
            p_output,
            &m_p_swap_chain1),
        TRAFFICMONITOR_ERROR_STR("Create swap chain for composition failed."));

    CallFunctionForEachResource<TRAFFICMONITOR_DV_V_ARGS(&Resource::OnSwapChainResizeEnd)>(m_resource_tracker, m_p_swap_chain1);
}

auto CDxgiSwapChain1::GetStorage()
    -> std::shared_ptr<Storage>
{
    return m_resource_tracker.GetSharedResourceTrackerStorage();
}

void CDxgiSwapChain1::Resize(std::uint32_t width, std::uint32_t height)
{
    CallFunctionForEachResource<TRAFFICMONITOR_DV_V_ARGS(&Resource::OnSwapChainResizeBegin)>(m_resource_tracker);

    ThrowIfFailed<CDxgiException>(
        m_p_swap_chain1->ResizeBuffers(
            0,
            width,
            height,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            0),
        TRAFFICMONITOR_ERROR_STR("Resize swap chain failed."));

    CallFunctionForEachResource<TRAFFICMONITOR_DV_V_ARGS(&Resource::OnSwapChainResizeEnd)>(m_resource_tracker, m_p_swap_chain1);
}

bool CDxgi1Support2::CheckSupport()
{
    const static auto result =
        FunctionChecker::CheckFunctionExist(_T("dxgi.dll"), "CreateDXGIFactory2");
    return result;
}

IDXGIFactory2* CDxgi1Support2::GetFactory()
{
    static auto result = MakeStaticVariableWrapper<IDXGIFactory2*>(
        [](auto pp_factory)
        {
            *pp_factory = nullptr;
            auto flags = 0;
#ifdef DEBUG
            flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
            ThrowIfFailed<CDxgiException>(
                CDllFunctions::CreateDXGIFactory2(
                    flags,
                    IID_PPV_ARGS(pp_factory)),
                TRAFFICMONITOR_ERROR_STR("Create dxgi factory2 failed."));
        },
        [](auto pp_factory)
        { RELEASE_COM(*pp_factory); });

    return result.Get();
}
