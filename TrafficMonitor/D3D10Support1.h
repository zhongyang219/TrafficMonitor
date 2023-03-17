#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include <memory>
#include <mutex>
#include <d3d10_1.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include "HResultException.h"
#include "RenderAPISupport.h"

class CD3D10Exception1 final : public CHResultException
{
    using CHResultException::CHResultException;
};

class CD3D10Device1
{
public:
    using Type = Microsoft::WRL::ComPtr<ID3D10Device1>;
    struct Data
    {
        Microsoft::WRL::ComPtr<ID3D10Device1> m_p_device1{};

        Microsoft::WRL::ComPtr<IDXGIAdapter1> m_p_adapter1{};
        D3D10_DRIVER_TYPE m_driver_type{D3D10_DRIVER_TYPE_HARDWARE};
        HMODULE m_h_software{NULL};
        UINT m_flags{0};
        D3D10_FEATURE_LEVEL1 m_feature_level{D3D10_FEATURE_LEVEL_10_1};
        UINT m_sdk_version{D3D10_1_SDK_VERSION};
    };
    using Resource = CDeviceResource<CD3D10Device1>;
    using Storage = MutipleStorage<Data,
                                   storage_t<CResourceTracker<Resource>>>;

private:
    std::shared_ptr<Storage> m_sp_storage{std::make_shared<Storage>()};
    CResourceTracker<Resource> m_resource_tracker{m_sp_storage};

public:
    void Recreate(Microsoft::WRL::ComPtr<IDXGIAdapter1> p_adapter1);
    auto Get() noexcept
        -> Microsoft::WRL::ComPtr<ID3D10Device1>;

    auto GetAdapter() noexcept
        -> Microsoft::WRL::ComPtr<IDXGIAdapter1>;

    auto GetDriverType() const noexcept
        -> D3D10_DRIVER_TYPE;
    auto SetDriverType(D3D10_DRIVER_TYPE driver_type) noexcept
        -> CD3D10Device1&;

    auto GetSoftwareHandle() const noexcept
        -> HMODULE;
    auto SetSoftwareHandle(HMODULE h_software) noexcept
        -> CD3D10Device1&;

    auto GetFlags() const noexcept
        -> UINT;
    auto SetFlags(UINT flags) noexcept
        -> CD3D10Device1&;

    auto GetFeaturelLevel() const noexcept
        -> D3D10_FEATURE_LEVEL1;
    auto SetFeaturelLevel(D3D10_FEATURE_LEVEL1 featurel_level) noexcept
        -> CD3D10Device1&;

    auto GetSdkVersion() const noexcept
        -> UINT;
    auto SetSdkVersion(UINT sdk_version) noexcept
        -> CD3D10Device1&;

    auto GetStorage() noexcept
        -> std::shared_ptr<Storage>;
};

class CD3D10Support1
{
public:
    static bool CheckSupport();
    static auto GetDeviceList(bool force_refresh = false)
        -> const std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>>&;
};

class CDXShaderException final : public CHResultException
{
private:
    std::string m_error;

public:
    CDXShaderException(HRESULT hr, const char* p_error, Microsoft::WRL::ComPtr<ID3DBlob> p_shader_error);
    ~CDXShaderException() override = default;

    const char* what() const noexcept override;
};

struct ShaderMacro
{
    std::string m_name{};
    std::string m_definition{};
};

class CShader
{
private:
    std::string m_code{};
    std::string m_entry_point{};
    std::string m_name{};
    std::string m_target{};
    std::vector<ShaderMacro> m_macros{};
    mutable std::vector<D3D_SHADER_MACRO> m_shader_macros{0};
    ID3DInclude* m_p_include{};
    UINT m_flags1{};
    UINT m_flags2{};
    mutable bool m_is_macro_changed{true};
    mutable bool m_is_config_changed{true};
    mutable Microsoft::WRL::ComPtr<ID3DBlob> m_p_cached_byte_code{};

public:
    CShader() = default;
    ~CShader() = default;

    auto GetCode() const noexcept
        -> const std::string&;
    auto SetCode(const std::string& code)
        -> CShader&;

    auto GetEntryPoint() const noexcept
        -> const std::string&;
    auto SetEntryPoint(const std::string& entry_point)
        -> CShader&;

    auto GetName() const noexcept
        -> const std::string&;
    auto SetName(const std::string& name)
        -> CShader&;

    auto GetTarget() const noexcept
        -> const std::string&;
    auto SetTarget(const std::string& target)
        -> CShader&;

    auto AddMacro(const ShaderMacro& macro)
        -> CShader&;
    auto DeleteMacro(const std::string& name)
        -> CShader&;
    auto GetMacros() const noexcept
        -> const std::vector<ShaderMacro>&;

    UINT GetFlags1() const noexcept;
    auto SetFlags1(UINT flags1) noexcept
        -> CShader&;

    UINT GetFlags2() const noexcept;
    auto SetFlags2(UINT flags2) noexcept
        -> CShader&;

    auto GetInclude() noexcept
        -> ID3DInclude*;
    auto SetInclude(ID3DInclude* p_indclude) noexcept
        -> CShader&;

    auto Compile() const
        -> Microsoft::WRL::ComPtr<ID3DBlob>;
};

class CD3D10DrawCallWaiter
{
private:
    Microsoft::WRL::ComPtr<ID3D10Query> m_p_query{};

public:
    explicit CD3D10DrawCallWaiter(Microsoft::WRL::ComPtr<ID3D10Device1> p_device);
    ~CD3D10DrawCallWaiter() = default;
    /**
     * @brief
     *
     * @return HRESULT 若成功，返回S_OK，否则可能为DXGI_ERROR_DEVICE_REMOVED或DXGI_ERROR_INVALID_CALL
     * 若返回S_FALSE，此为函数本身的返回值，不能参考MS的文档，这代表等待超时（超过1.5s的等待时间）
     */
    HRESULT Wait() const noexcept;
};
