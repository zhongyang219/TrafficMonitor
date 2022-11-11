#pragma once
#include <type_traits>
#include <memory>
#include <unordered_set>

#define TRAFFICMONITOR_HAS_FUNCTION_CALL(function_name, class_name) \
    ::is_function_##function_name##_exist<class_name>::value
#define TRAFFICMONITOR_DEFINE_HAS_FUNCTION_CALL(function_name)                 \
    template <typename T>                                                      \
    class is_function_##function_name##_exist                                  \
    {                                                                          \
        using Existence = char;                                                \
        struct Inexistence                                                     \
        {                                                                      \
            char unused[2];                                                    \
        };                                                                     \
                                                                               \
        template <typename C>                                                  \
        static Existence test(decltype(&C::function_name));                    \
        template <typename C>                                                  \
        static Inexistence test(...);                                          \
                                                                               \
    public:                                                                    \
        constexpr static bool value = sizeof(test<T>(0)) == sizeof(Existence); \
    }

TRAFFICMONITOR_DEFINE_HAS_FUNCTION_CALL(AddResource);
TRAFFICMONITOR_DEFINE_HAS_FUNCTION_CALL(RemoveResource);
TRAFFICMONITOR_DEFINE_HAS_FUNCTION_CALL(Recreate);

template <class... Ts>
struct MutipleStorage : public Ts...
{
};

template <class T>
using storage_t = typename T::Storage;

template <class ResourceTracker, class Device>
void NotifyAllResourceWhenDeviceRecreate(ResourceTracker& tracker, Device& device)
{
    auto ref_track_set = tracker.GetTrackerSet();
    for (auto&& it : ref_track_set)
    {
        it->OnDeviceRecreate(device);
    }
}

/**
 * @brief CRTP 基类
 *
 * @tparam Device 是子类
 */
template <class Device>
class CTrackableDevice
{
    static_assert(!std::is_pointer<Device>{}, "Type Device should not be a pointer type.");
    using DevicePointer = std::add_pointer_t<Device>;

public:
    struct Storage
    {
        DevicePointer m_p_device;
        auto GetDevicePointer()
            -> DevicePointer&
        {
            return m_p_device;
        }
    };

private:
    std::shared_ptr<Storage> m_sp_storage;

public:
    CTrackableDevice()
        : m_sp_storage{}
    {
        auto p_device = static_cast<DevicePointer>(this);
        m_sp_storage = std::make_shared<Storage>(p_device);
    }
    CTrackableDevice(std::shared_ptr<Storage> sp_storage)
        : m_sp_storage{sp_storage}
    {
    }
    ~CTrackableDevice() = default;
    CTrackableDevice(const CTrackableDevice&)
        : m_sp_storage{}
    {
        auto p_device = static_cast<DevicePointer>(this);
        m_sp_storage = std::make_shared<Storage>(p_device);
    }
    CTrackableDevice& operator=(const CTrackableDevice&)
    {
        auto p_device = static_cast<DevicePointer>(this);
        this->m_sp_storage = std::make_shared<Storage>(p_device);
        return *this;
    }
    CTrackableDevice(CTrackableDevice&& other) noexcept
        : m_sp_storage{std::exchange(other.m_sp_storage, nullptr)}
    {
        m_sp_storage.GetDevicePointer() = static_cast<DevicePointer>(this);
    }
    CTrackableDevice& operator=(CTrackableDevice&& other) noexcept
    {
        m_sp_storage = std::exchange(other.m_sp_storage, nullptr);
        m_sp_storage.GetDevicePointer() = static_cast<DevicePointer>(this);
        return *this;
    }

    auto GetSharedDevicePointerStorage() const
        -> std::shared_ptr<Storage>
    {
        return m_sp_storage;
    }
};

template <class Resource>
class CResourceTracker
{
    using ResourcePointer = std::add_pointer_t<Resource>;

public:
    struct Storage
    {
        std::unordered_set<ResourcePointer> m_resource_pointer_set{};

        auto GetResourcePointerSet()
            -> std::unordered_set<ResourcePointer>&
        {
            return m_resource_pointer_set;
        }
        void AddResource(ResourcePointer p_resource)
        {
            auto count = m_resource_pointer_set.count(p_resource);
            if (count == 0)
            {
                m_resource_pointer_set.insert(p_resource);
            }
        }
        void RemoveResource(ResourcePointer p_resource)
        {
            m_resource_pointer_set.erase(p_resource);
        }
    };

private:
    std::shared_ptr<Storage> m_sp_storage;

public:
    CResourceTracker()
        : m_sp_storage{std::make_shared<CResourceTracker>()}
    {
    }
    CResourceTracker(std::shared_ptr<Storage> m_sp_storage)
        : m_sp_storage{m_sp_storage}
    {
    }
    auto GetTrackerSet() const noexcept
        -> const std::unordered_set<ResourcePointer>&
    {
        return m_sp_storage->GetResourcePointerSet();
    }
    auto GetSharedResourceTrackerStorage()
        -> std::shared_ptr<Storage>
    {
        return m_sp_storage;
    }
};

template <class Device>
class CDeviceResource
{
public:
    using DeviceType = typename Device::Type;
    using Storage = typename Device::Storage;

private:
    static_assert(TRAFFICMONITOR_HAS_FUNCTION_CALL(AddResource, Storage), "Function Storage::AddResource not found.");
    static_assert(TRAFFICMONITOR_HAS_FUNCTION_CALL(RemoveResource, Storage), "Function Storage::RemoveResource not found.");

    std::weak_ptr<Storage> m_wp_device_storage;

    static void BeginTrack(CDeviceResource& device_resource)
    {
        auto sp_device_storage = device_resource.m_wp_device_storage.lock();
        if (sp_device_storage)
        {
            sp_device_storage->AddResource(std::addressof(device_resource));
        }
    }
    static void EndTrack(CDeviceResource& device_resource)
    {
        auto sp_device_storage = device_resource.m_wp_device_storage.lock();
        if (sp_device_storage)
        {
            sp_device_storage->RemoveResource(std::addressof(device_resource));
        }
    }

public:
    CDeviceResource(const std::shared_ptr<Storage> sp_device_storage)
        : m_wp_device_storage{sp_device_storage}
    {
        BeginTrack(*this);
    }
    ~CDeviceResource()
    {
        EndTrack(*this);
    }
    CDeviceResource(const CDeviceResource& other)
        : m_wp_device_storage{other.m_wp_device_storage}
    {
        BeginTrack(*this);
    }
    CDeviceResource& operator=(CDeviceResource& other)
    {
        this->m_wp_device_storage = other.m_wp_device;
        BeginTrack(*this);
        return *this;
    }
    CDeviceResource(CDeviceResource&& other)
        : m_wp_device_storage{}
    {
        EndTrack(other);
        m_wp_device_storage = std::exchange(other.m_wp_device_storage, nullptr);
        BeginTrack(*this);
    }
    CDeviceResource& operator=(CDeviceResource&& other)
    {
        EndTrack(other);
        this->m_wp_device_storage = std::exchange(other.m_wp_device_storage, nullptr);
        BeginTrack(*this);
        return *this;
    }

    virtual void OnDeviceRecreate(DeviceType new_device) noexcept = 0;

    /**
     * @brief 当Device不是继承自CTrackableDevice<Device>时
     （即Device::Storage不能转换为CTrackableDevice<Device>::Storage时），不能在代码中调用此函数
     *
     */
    void RequestDeviceRecreate()
    {
        auto sp_device_storage = m_wp_device_storage.lock();
        if (sp_device_storage)
        {
            auto sp_trackable_device = std::static_pointer_cast<typename CTrackableDevice<Device>::Storage>(sp_device_storage);
            auto p_device = sp_trackable_device->GetDevicePointer();
            p_device->Recreate();
        }
    }
};
