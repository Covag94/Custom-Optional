#pragma once
#include <utility>

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
    T *ptr() { return reinterpret_cast<T *>(&m_storage); }

    const T *ptr() const { return reinterpret_cast<const T *>(&m_storage); }

    void swap(Optional &other) noexcept
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

private:
    void reset() noexcept
    {
        if (m_hasValue)
        {
            ptr()->~T();
            m_hasValue = false;
        }
    }

public:
    // Optional ctor for nullopt
    Optional(nullopt_t) : m_hasValue(false) {}

    // Optional in-place ctor
    template <typename... Args>
    Optional(in_place_t, Args &&...args) : m_hasValue(false)
    {
        new (ptr()) T(std::forward<Args>(args)...);
    }

    Optional() : m_hasValue(false)
    {
    }
    ~Optional()
    {
        if (m_hasValue)
        {
            ptr()->~T();
        }
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

    template <typename... Args>
    void emplace(Args &&...args)
    {
        if (m_hasValue)
        {
            ptr()->~T();

            // Need to reset before construction
            m_hasValue = false;
        }
        new (ptr()) T(std::forward<Args>(args)...);
        m_hasValue = true;
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

    Optional &operator=(nullopt_t) noexcept
    {
        reset();
        return *this;
    }

    Optional(Optional &&other) noexcept : m_hasValue(false)
    {
        if (other.m_hasValue)
        {
            new (ptr()) T(std::move(*other.ptr()));
            m_hasValue = other.m_hasValue;
            other.reset();
        }
    }

    Optional &operator=(Optional &&other) noexcept
    {
        swap(other);
        return *this;
    }

public:
    [[nodiscard]] bool has_value() const { return m_hasValue; }
    [[nodiscard]] explicit operator bool() const noexcept { return m_hasValue; }
    [[nodiscard]] T &operator*() { return *ptr(); }
    [[nodiscard]] const T &operator*() const { return *ptr(); }
    [[nodiscard]] T *operator->() { return ptr(); }
    [[nodiscard]] const T *operator->() const { return ptr(); }
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
};