#pragma once
#include <utility>
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

template <class T>
using resource_t = typename T::Resource;

template <class T>
struct get_first_template_argument;
template <template <class> class T1, class T2>
struct get_first_template_argument<T1<T2>>
{
    using type = T2;
};
template <class T>
using get_first_template_argument_t = typename get_first_template_argument<T>::type;

template <class T>
struct resolve_member_function;
template <class R, class C>
struct resolve_member_function<R C::*>
{
    using member_type = R;
    using class_type = C;
};
template<class T>
using get_member_class_t = typename resolve_member_function<T>::class_type;
template <class T>
using get_member_t = typename resolve_member_function<T>::member_type;

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

template <class Device>
class CDeviceResourceBase;

template <class Resource>
class CResourceTracker
{
    /**
     * @brief 将CDeviceResource<Device>变换为CDeviceResourceBase<Device>*
     *
     */
    using ResourcePointer =
        std::add_pointer_t<
            CDeviceResourceBase<
                get_first_template_argument_t<Resource>>>;

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

#define TRAFFICMONITOR_DV_V_ARGS(v) decltype(v), v

template <class T, T PClassFunction, class Resource, class... Args>
void CallFunctionForEachResource(CResourceTracker<Resource>& tracker, Args&&... args)
{
    static_assert(!std::is_base_of<decltype(*tracker.GetTrackerSet().begin()), Resource>{},
                  "Type Resource is not derived from std::remove_pointer_t<CResourceTracker<Resource>::ResourcePointer>");

    auto ref_track_set = tracker.GetTrackerSet();
    for (auto&& it : ref_track_set)
    {
        auto* it_impl = static_cast<Resource*>(it);
        (it_impl->*PClassFunction)(std::forward<Args>(args)...);
    }
}

template <class Resource, class Device>
void NotifyAllResourceWhenDeviceRecreate(CResourceTracker<Resource>& tracker, Device& device)
{
    CallFunctionForEachResource<TRAFFICMONITOR_DV_V_ARGS(&Resource::OnDeviceRecreate)>(tracker, device);
}

template <class Device>
class CDeviceResourceBase
{
public:
    using DeviceType = typename Device::Type;
    using Storage = typename Device::Storage;

private:
    static_assert(TRAFFICMONITOR_HAS_FUNCTION_CALL(AddResource, Storage), "Function Storage::AddResource not found.");
    static_assert(TRAFFICMONITOR_HAS_FUNCTION_CALL(RemoveResource, Storage), "Function Storage::RemoveResource not found.");

    std::weak_ptr<Storage> m_wp_device_storage;

    static void BeginTrack(CDeviceResourceBase& device_resource)
    {
        auto sp_device_storage = device_resource.m_wp_device_storage.lock();
        if (sp_device_storage)
        {
            sp_device_storage->AddResource(std::addressof(device_resource));
        }
    }
    static void EndTrack(CDeviceResourceBase& device_resource)
    {
        auto sp_device_storage = device_resource.m_wp_device_storage.lock();
        if (sp_device_storage)
        {
            sp_device_storage->RemoveResource(std::addressof(device_resource));
        }
    }

public:
    CDeviceResourceBase(const std::shared_ptr<Storage> sp_device_storage)
        : m_wp_device_storage{sp_device_storage}
    {
        BeginTrack(*this);
    }

protected:
    ~CDeviceResourceBase()
    {
        EndTrack(*this);
    }

public:
    CDeviceResourceBase(const CDeviceResourceBase& other)
        : m_wp_device_storage{other.m_wp_device_storage}
    {
        BeginTrack(*this);
    }
    CDeviceResourceBase& operator=(CDeviceResourceBase& other)
    {
        this->m_wp_device_storage = other.m_wp_device;
        BeginTrack(*this);
        return *this;
    }
    CDeviceResourceBase(CDeviceResourceBase&& other)
        : m_wp_device_storage{}
    {
        EndTrack(other);
        m_wp_device_storage = std::exchange(other.m_wp_device_storage, nullptr);
        BeginTrack(*this);
    }
    CDeviceResourceBase& operator=(CDeviceResourceBase&& other)
    {
        EndTrack(other);
        this->m_wp_device_storage = std::exchange(other.m_wp_device_storage, nullptr);
        BeginTrack(*this);
        return *this;
    }
};

template <class Device>
class CDeviceResource : public CDeviceResourceBase<Device>
{
private:
    using Base = CDeviceResourceBase<Device>;

public:
    using Base::Base;
    using typename Base::DeviceType;

    virtual void OnDeviceRecreate(DeviceType new_device) noexcept = 0;
};
