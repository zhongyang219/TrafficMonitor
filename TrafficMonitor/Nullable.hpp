#pragma once
#include <type_traits>

template <class T>
using std_aligned_storage = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

//皆不保证多线程操作安全
/**
 * @brief Nullable<T>的默认删除器，注意重载operator()的参数为T*
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
class Nullable
{
public:
    Nullable(Deleter deleter = {})
        : m_has_value_and_ebo_deleter{deleter} {}
    ~Nullable()
    {
        if (m_has_value_and_ebo_deleter)
        {
            DestroySelf();
        }
    }
    Nullable(const Nullable&) = delete;
    Nullable operator=(const Nullable&) = delete;
    template <class... Args>
    void Construct(Args&&... args)
    {
        if (m_has_value_and_ebo_deleter)
        {
            DestroySelf();
            m_has_value_and_ebo_deleter.SetHasValue(false);
        }
        ::new (&m_storage) T(std::forward<Args>(args)...);
        m_has_value_and_ebo_deleter.SetHasValue(true);
    }
    const T& GetUnsafe() const noexcept
    {
        return *static_cast<const T*>(static_cast<const void*>(std::addressof(m_storage)));
    }
    T& GetUnsafe() noexcept
    {
        return *static_cast<T*>(static_cast<void*>(std::addressof(m_storage)));
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
    operator bool() const noexcept
    {
        return m_has_value_and_ebo_deleter;
    }

private:
    void DestroySelf()
    {
        static_cast<Deleter>(m_has_value_and_ebo_deleter).operator()(&Get());
    }
    class CallNullObjectError : public std::runtime_error
    {
    public:
        CallNullObjectError()
            : std::runtime_error{"Value is uninitialized!"} {}
        ~CallNullObjectError() override = default;
    };
    void Check() const
    {
        if (!m_has_value_and_ebo_deleter)
        {
            throw CallNullObjectError{};
        }
    }

    std_aligned_storage<T> m_storage{};
    class HasValueAndEboDeleter : public Deleter
    {
    public:
        HasValueAndEboDeleter(Deleter deleter) : Deleter(deleter) {}
        ~HasValueAndEboDeleter() = default;
        operator bool() const noexcept
        {
            return m_has_value;
        }
        void SetHasValue(bool value)
        {
            m_has_value = value;
        }

    private:
        bool m_has_value{false};
    } m_has_value_and_ebo_deleter{};
};
template <class T, class Deleter = NullableDefaultDeleter<T>>
auto MakeNullableObject(Deleter deleter)
    -> Nullable<T, Deleter>
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
class LazyConstructable
{
public:
    LazyConstructable() = default;
    ~LazyConstructable() = default;
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
    Nullable<T, Deleter> m_content{};
    bool m_is_available{false};
};
