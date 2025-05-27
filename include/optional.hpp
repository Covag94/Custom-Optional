#pragma once

template <typename T>
class Optional
{
private:
    alignas(T) unsigned char m_storage[sizeof(T)];
    bool m_hasValue;

    // Get pointer to buffer
    T *ptr() { return reinterpret_cast<T *>(&m_storage); }

    const T *ptr() const { return reinterpret_cast<const T *>(&m_storage); }

public:
    Optional() : m_hasValue(false) {}
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

    Optional &operator=(const Optional &other)
    {
        if (this != &other)
        {
            if (m_hasValue && other.m_hasValue)
            {
                *ptr() = *other.ptr();
            }
            else if (m_hasValue && !other.m_hasValue)
            {
                ptr()->~T();
                m_hasValue = other.m_hasValue;
            }
            else if (!m_hasValue && other.m_hasValue)
            {
                new (ptr()) T(*other.ptr());
                m_hasValue = other.m_hasValue;
            }
        }

        return *this;
    }

    Optional(Optional &&other) noexcept : m_hasValue(false)
    {
        if (other.m_hasValue)
        {
            new (ptr()) T(std::move(*other.ptr()));
            m_hasValue = other.m_hasValue;
            other.ptr()->~T();
            other.m_hasValue = false;
        }
    }

    Optional &operator=(Optional &&other) noexcept
    {
        if (this != &other)
        {
            if (m_hasValue && other.m_hasValue)
            {
                *ptr() = std::move(*other.ptr());
            }
            else if (m_hasValue && !other.m_hasValue)
            {
                ptr()->~T();
                m_hasValue = other.m_hasValue;
            }
            else if (!m_hasValue && other.m_hasValue)
            {
                new (ptr()) T(std::move(*other.ptr()));
                m_hasValue = other.m_hasValue;
            }
        }

        if (other.m_hasValue)
        {
            other.ptr()->~T();
            other.m_hasValue = false;
        }

        return *this;
    }

public:
    bool has_value() const { return m_hasValue; }
};