#pragma once
#include <dxgi1_3.h>
#include "HResultException.h"
#include "RenderAPISupport.h"

#pragma comment(lib, "DXGI.lib")

class CDxgiException final : public CHResultException
{
    using CHResultException::CHResultException;
};

template <class SwapChain>
class CDxgiSwapChainResource : public CDeviceResourceBase<SwapChain>
{
public:
    using Base = CDeviceResourceBase<SwapChain>;
    using Base::Base;
    using typename Base::DeviceType;

    /**
     * @brief 在准备Resize交换链时，此函数将被调用，用于释放所有和交换链相关的资源。\n
     注意：此函数可能被多次调用，且调用次数不一定与OnSwapChainResizeEnd的次数相匹配，
     因此必须保证资源可以被反复执行释放操作
     *
     */
    virtual void OnSwapChainResizeBegin() noexcept = 0;
    /**
     * @brief 在交换链完成Resize后，此函数将被调用，用于重建所有和交换链相关的资源
     *
     */
    virtual void OnSwapChainResizeEnd(DeviceType p_resized_swap_chain) noexcept = 0;
};

/**
 * @brief 此类对应的资源为CDxgiSwapChainResource<CDxgiSwapChain1>，而非CDeviceResource<CDxgiSwapChain1>
 *
 */
class CDxgiSwapChain1
{
public:
    using Resource = CDxgiSwapChainResource<CDxgiSwapChain1>;
    using Type = Microsoft::WRL::ComPtr<IDXGISwapChain1>;
    using Storage = storage_t<CResourceTracker<Resource>>;

private:
    Type m_p_swap_chain1{};
    CResourceTracker<Resource> m_resource_tracker{std::make_shared<Storage>()};

public:
    void Recreate(Microsoft::WRL::ComPtr<IUnknown> p_device, const DXGI_SWAP_CHAIN_DESC1& ref_desc1, IDXGIOutput* p_output = nullptr);
    auto GetStorage()
        -> std::shared_ptr<Storage>;
    void Resize(std::uint32_t width, std::uint32_t height);
};

class CDxgi1Support2
{
public:
    static bool CheckSupport();
    static IDXGIFactory2* GetFactory();
};
