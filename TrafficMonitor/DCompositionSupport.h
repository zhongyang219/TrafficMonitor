#pragma once
#include <wrl/client.h>
#include <dcomp.h>
#include "HResultException.h"
#include "RenderAPISupport.h"

class CDCompositionException final : public CHResultException
{
    using CHResultException::CHResultException;
};

class CDCompositionDevice
{
public:
    using Resource = CDeviceResource<CDCompositionDevice>;
    using Type = Microsoft::WRL::ComPtr<IDCompositionDevice>;
    using Storage = storage_t<CResourceTracker<Resource>>;

private:
    Type m_p_device{};
    CResourceTracker<Resource> m_resource_tracker{std::make_shared<Storage>()};

public:
    void Recreate(Microsoft::WRL::ComPtr<IDXGIDevice> p_dxgi_device);
    auto GetStorage()
        -> std::shared_ptr<Storage>;
    auto Get()
        -> Microsoft::WRL::ComPtr<IDCompositionDevice>;
};

class CDCompositionSupport
{
public:
    static bool CheckSupport();
};
