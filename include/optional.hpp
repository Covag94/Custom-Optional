#pragma once
#include <utility>
#include <type_traits>
#include <cassert>
#include <stdexcept>

struct nullopt_t
{
    explicit constexpr nullopt_t(int) {}
};
inline constexpr nullopt_t nullopt{0};

struct in_place_t
{
    explicit constexpr in_place_t() = default;
};
inline constexpr in_place_t in_place{};

template <typename T>
class Optional
{
private:
    alignas(T) unsigned char m_storage[sizeof(T)];
    bool m_hasValue;

    // Get pointer to buffer
    T *ptr() noexcept { return reinterpret_cast<T *>(&m_storage); }

    const T *ptr() const noexcept { return reinterpret_cast<const T *>(&m_storage); }

    void swap(Optional &other) noexcept(std::is_nothrow_destructible_v<T> &&
                                        std::is_nothrow_move_constructible_v<T> && std::is_nothrow_swappable_v<T>)
    {
        if (m_hasValue && other.m_hasValue)
        {
            std::swap(*ptr(), *other.ptr());
        }
        else if (m_hasValue && !other.m_hasValue)
        {
            new (other.ptr()) T(std::move(*ptr()));
            ptr()->~T();
            std::swap(m_hasValue, other.m_hasValue);
        }
        else if (!m_hasValue && other.m_hasValue)
        {
            new (ptr()) T(std::move(*other.ptr()));
            other.ptr()->~T();
            std::swap(m_hasValue, other.m_hasValue);
        }
    }

public:
    //
    // Constructors
    //

    // Optional ctor for nullopt
    constexpr Optional(nullopt_t) : m_hasValue(false) {}
    constexpr Optional() : m_hasValue(false)
    {
    }

    // Optional in-place ctor
    template <typename... Args>
    Optional(in_place_t, Args &&...args) : m_hasValue(false)
    {
        new (ptr()) T(std::forward<Args>(args)...);
    }
    Optional(const T &value) : m_hasValue(false)
    {
        new (ptr()) T(value);
        m_hasValue = true;
    }
    Optional(T &&value) : m_hasValue(false)
    {
        new (ptr()) T(std::move(value));
        m_hasValue = true;
    }

    ~Optional()
    {
        if (m_hasValue)
        {
            ptr()->~T();
        }
    }

    template <typename... Args>
    T &emplace(Args &&...args)
    {
        if (m_hasValue)
        {
            ptr()->~T();

            // Need to reset before construction
            m_hasValue = false;
        }
        new (ptr()) T(std::forward<Args>(args)...);
        m_hasValue = true;

        return *ptr();
    }

    Optional(const Optional &other) : m_hasValue(false)
    {
        if (other.m_hasValue)
        {
            new (ptr()) T(*other.ptr());

            // Only set to true after placement
            // new has succeeded
            m_hasValue = true;
        }
    }

    Optional &operator=(Optional other)
    {
        swap(other);
        return *this;
    }

    Optional &operator=(nullopt_t) noexcept(std::is_nothrow_destructible_v<T>)
    {
        reset();
        return *this;
    }

    Optional(Optional &&other) noexcept(std::is_nothrow_move_constructible_v<T>) : m_hasValue(false)
    {
        if (other.m_hasValue)
        {
            new (ptr()) T(std::move(*other.ptr()));
            m_hasValue = other.m_hasValue;
            other.reset();
        }
    }

    Optional &operator=(Optional &&other) noexcept(std::is_nothrow_destructible_v<T> &&
                                                   std::is_nothrow_move_constructible_v<T> && std::is_nothrow_swappable_v<T>)
    {
        swap(other);
        return *this;
    }

    bool operator==(nullopt_t) const noexcept { return !m_hasValue; }
    bool operator!=(nullopt_t) const noexcept { return m_hasValue; }

    bool operator==(const Optional &other) const noexcept
    {
        if (m_hasValue != other.m_hasValue)
        {
            return false;
        }
        if (!m_hasValue)
        {
            return true;
        }
        return *ptr() == *other.ptr();
    }
    bool operator!=(const Optional &other) const noexcept
    {
        return !(*this == other);
    }

    void reset() noexcept(std::is_nothrow_destructible_v<T>)
    {
        if (m_hasValue)
        {
            ptr()->~T();
            m_hasValue = false;
        }
    }

public:
    [[nodiscard]] constexpr bool has_value() const { return m_hasValue; }
    [[nodiscard]] explicit constexpr operator bool() const noexcept { return m_hasValue; }
    [[nodiscard]] T &operator*()
    {
        assert(m_hasValue);
        return *ptr();
    }
    [[nodiscard]] const T &operator*() const
    {
        assert(m_hasValue);
        *ptr();
    }
    [[nodiscard]] T *operator->()
    {
        assert(m_hasValue);
        return ptr();
    }
    [[nodiscard]] const T *operator->() const
    {
        assert(m_hasValue);
        return ptr();
    }
    [[nodiscard]] T &value()
    {
        if (!m_hasValue)
            throw std::bad_optional_access();
        return *ptr();
    }
    [[nodiscard]] const T &value() const
    {
        if (!m_hasValue)
            throw std::bad_optional_access();
        return *ptr();
    }

    template <typename U>
    constexpr T value_or(U &&default_value) const & // lvalue overload
    {
        static_assert(std::is_copy_constructible_v<T> && std::is_convertible_v<U &&, T>);
        return m_hasValue ? **this : static_cast<T>(std::forward<U>(default_value));
    }

    template <typename U>
    constexpr T value_or(U &&default_value) && // rvalue overload
    {
        static_assert(std::is_move_constructible_v<T> && std::is_convertible_v<U &&, T>);
        return m_hasValue ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
    }
};