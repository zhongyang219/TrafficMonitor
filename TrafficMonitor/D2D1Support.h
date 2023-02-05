#pragma once
#include <d2d1_1.h>
#include <dwrite.h>
#include "HResultException.h"
#include "RenderAPISupport.h"

class CD2D1Exception final : public CHResultException
{
    using CHResultException::CHResultException;
};

class CDWriteException final : public CHResultException
{
    using CHResultException::CHResultException;
};

class CD2D1Support
{
public:
#ifdef DEBUG
    constexpr static D2D1_FACTORY_OPTIONS CREATION_OPTIONS{D2D1_DEBUG_LEVEL_INFORMATION};
#else
    constexpr static D2D1_FACTORY_OPTIONS CREATION_OPTIONS{D2D1_DEBUG_LEVEL_NONE};
#endif

    static bool CheckSupport();
    static ID2D1Factory* GetFactory();
};

class CD2D1Device
{
public:
    using Type = Microsoft::WRL::ComPtr<ID2D1Device>;
    using Resource = CDeviceResource<CD2D1Device>;
    using Storage = storage_t<CResourceTracker<Resource>>;

private:
    Type m_p_device{};
    CResourceTracker<Resource> m_resource_tracker{std::make_shared<Storage>()};

public:
    void Recreate(Microsoft::WRL::ComPtr<IDXGIDevice> p_dxgi_device);
    auto GetStorage()
        -> std::shared_ptr<Storage>;
    Type Get();
};

class CD2D1Support1
{
public:
    static bool CheckSupport();
    static ID2D1Factory1* GetFactory();
};

class CDWriteSupport
{
public:
    static bool CheckSupport();
    static IDWriteFactory* GetFactory();
};
