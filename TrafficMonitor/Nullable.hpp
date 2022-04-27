#pragma once
#include <type_traits>

template <class T>
using std_aligned_storage = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

template<class T>
struct NullableDefaultDeleter
{
    void operator()(T* p_this)
    {
        p_this->~T();
    }
};
template <class T, class Deleter = NullableDefaultDeleter<T>>
class Nullable
{
public:
    Nullable(Deleter deleter = {})
        : m_has_value_and_ebo_deleter{ deleter } {}
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
    const T& GetUnsafe()const noexcept
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
    void Check() const
    {
        if (!m_has_value_and_ebo_deleter)
        {
            throw std::runtime_error{ "Value is uninitialized!" };
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
        bool m_has_value{ false };
    } m_has_value_and_ebo_deleter{};
};
template <class T, class Deleter = NullableDefaultDeleter<T>>
Nullable<T, Deleter> MakeNullableObject(Deleter deleter)
{
    return { deleter };
}