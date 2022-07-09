#pragma once
#include <type_traits>

template <class T>
using std_aligned_storage = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

//皆不保证多线程操作安全
/**
 * @brief CNullable<T>的默认删除器，注意重载operator()的参数为T*
 *
 * @tparam T 要删除的类型
 */
template <class T>
struct NullableDefaultDeleter
{
    void operator()(T* p_this)
    {
        p_this->~T();
    }
};
/**
 * @brief 可空对象，允许对象在无默认构造函数的情况下仍然可以执行默认构造，代价是必须在使用对象之前执行Construct方法
 *
 * @tparam T 实际存储的对象
 * @tparam Deleter T的删除器
 */
template <class T, class Deleter = NullableDefaultDeleter<T>>
class CNullable
{
    using StorageType = std_aligned_storage<T>;
    template<class C, class... Args>
    static void EmplaceAt(C* p_object, Args&&... args)
    {
        ::new (p_object) C(std::forward<Args>(args)...);
    }
public:
    CNullable(Deleter deleter = {})
        : m_storage{deleter} {}
    ~CNullable()
    {
        if (m_has_value)
        {
            DestroySelf();
        }
    }
    CNullable(const CNullable& other)
        :m_has_value{other.m_has_value}, m_storage{static_cast<Deleter>(other.m_storage)}
    {
        if (other)
        {
            EmplaceAt(&m_storage.GetUnsafe(), other.m_storage.GetUnsafe());
        }
    }
    CNullable& operator=(const CNullable& other)
    {
        this->m_has_value = other.m_has_value;
        Deleter& ref_deleter = m_storage;
        ref_deleter = static_cast<Deleter>(other.m_storage);
        if (other)
        {
            EmplaceAt(&m_storage.GetUnsafe(), other.m_storage.GetUnsafe());
        }
    }
    CNullable(CNullable&& other) noexcept
        :m_has_value{other.m_has_value}, m_storage{std::move(static_cast<Deleter>(other.m_storage))}
    {
        if (other)
        {
            EmplaceAt(&m_storage.GetUnsafe(), std::move(other.m_storage.GetUnsafe()));
        }
    }
    CNullable& operator=(CNullable&& other) noexcept
    {
        this->m_has_value = other.m_has_value;
        Deleter& ref_deleter = m_storage;
        ref_deleter = std::move(static_cast<Deleter>(other.m_storage));
        if (other)
        {
            EmplaceAt(&m_storage.GetUnsafe(), std::move(other.m_storage.GetUnsafe()));
        }
    }

    template <class... Args>
    void Construct(Args&&... args)
    {
        if (m_has_value)
        {
            DestroySelf();
            m_has_value = false;
        }

        try
        {
            EmplaceAt(&m_storage.GetUnsafe(), std::forward<Args>(args)...);
        }
        catch (...)
        {
            throw;
        }
        m_has_value = true;
    }
    const T& GetUnsafe() const noexcept
    {
        return m_storage.GetUnsafe();
    }
    T& GetUnsafe() noexcept
    {
        return m_storage.GetUnsafe();
    }
    const T& Get() const noexcept
    {
        Check();
        return GetUnsafe();
    }
    T& Get() noexcept
    {
        Check();
        return GetUnsafe();
    }
    bool HasValue() const noexcept
    {
        return m_has_value;
    }
    operator bool() const noexcept
    {
        return HasValue();
    }

private:
    void DestroySelf()
    {
        static_cast<Deleter>(m_storage).operator()(&Get());
    }
    void Check() const
    {
        if (!m_has_value)
        {
            throw CallNullObjectError{};
        }
    }

    class CallNullObjectError : public std::runtime_error
    {
    public:
        CallNullObjectError()
            : std::runtime_error{"Value is uninitialized!"} {}
        ~CallNullObjectError() override = default;
    };

    class StorageAndEboDeleter : public Deleter
    {
    private:
        StorageType m_storage;

        auto GetStoragePointer() const noexcept
            -> const StorageType*
        {
            return std::addressof(m_storage);
        }
        auto GetStoragePointer() noexcept
            -> StorageType*
        {
            return std::addressof(m_storage);
        }
    public:
        explicit StorageAndEboDeleter(Deleter deleter)
            :Deleter{deleter} {}
        ~StorageAndEboDeleter() = default;
        StorageAndEboDeleter(const StorageAndEboDeleter&) = delete;
        StorageAndEboDeleter& operator=(const StorageAndEboDeleter&) = delete;

        const T& GetUnsafe() const noexcept
        {
            return *static_cast<const T*>(static_cast<const void*>(GetStoragePointer()));
        }
        T& GetUnsafe() noexcept
        {
            return *static_cast<T*>(static_cast<void*>(GetStoragePointer()));
        }
    };
    bool m_has_value{false};
    StorageAndEboDeleter m_storage;
};
template <class T, class Deleter = NullableDefaultDeleter<T>>
auto MakeNullableObject(Deleter deleter)
    -> CNullable<T, Deleter>
{
    return {deleter};
}

/**
 * @brief 可延迟构造的对象，用于预先分配内存，在有使用该对象的请求时立即构造此对象，对象必须可默认构造
 *
 * @tparam T 要被应用这一特性的类型
 * @tparam Deleter T的删除器，默认为NullableDefaultDeleter<T>
 */
template <class T, class Deleter = NullableDefaultDeleter<T>>
class CLazyConstructable
{
public:
    CLazyConstructable() = default;
    ~CLazyConstructable() = default;
    T& Get()
    {
        if (m_is_available)
        {
            return m_content.GetUnsafe();
        }
        else
        {
            m_content.Construct();
            m_is_available = true;
            return m_content.GetUnsafe();
        }
    }

private:
    bool m_is_available{false};
    CNullable<T, Deleter> m_content{};
};
