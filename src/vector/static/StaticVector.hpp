/**
 * SPDX-FileCopyrightText: 2025 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#if defined(__SAMD21__) || defined(__SAMD51__) || \
    defined(__SAMD21G18A__) || defined(__SAMD21E18A__) || defined(__SAMD51J19A__) || defined(__SAMD51P20A__) || \
    defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKRWIFI1010)
    #include <new>
#endif

void *operator new(size_t, void *ptr);
void operator delete(void *, void *ptr);


namespace ReadyUtils
{
    template <typename T, size_t Capacity>
    inline T *StaticVector<T, Capacity>::get_ptr(size_t i)
    {
        return reinterpret_cast<T *>(data_storage + i * sizeof(T));
    }
    template <typename T, size_t Capacity>
    inline const T *StaticVector<T, Capacity>::get_ptr(size_t i) const
    {
        return reinterpret_cast<const T *>(data_storage + i * sizeof(T));
    }
    template <typename T, size_t Capacity>
    inline StaticVector<T, Capacity>::StaticVector() = default;
    template <typename T, size_t Capacity>
    inline StaticVector<T, Capacity>::~StaticVector()
    {
#ifdef STATIC_VECTOR_CLEANUP_ENABLED
        clear();
#endif
    }
    template <typename T, size_t Capacity>
    inline StaticVector<T, Capacity>::StaticVector(const StaticVector &other)
    {
        for (size_t i = 0; i < other.count; ++i)
        {
            new (static_cast<void *>(get_ptr(i))) T(*other.get_ptr(i)); // Copy construct
        }
        count = other.count;
    }
    template <typename T, size_t Capacity>
    inline StaticVector<T, Capacity> &StaticVector<T, Capacity>::operator=(const StaticVector &other)
    {
        if (this != &other)
        {
            clear();
            for (size_t i = 0; i < other.count; ++i)
            {
                new (static_cast<void *>(get_ptr(i))) T(*other.get_ptr(i)); // Copy construct
            }
            count = other.count;
        }
        return *this;
    }
    template <typename T, size_t Capacity>
    inline constexpr size_t StaticVector<T, Capacity>::size() const
    {
        return count;
    }
    template <typename T, size_t Capacity>
    inline bool StaticVector<T, Capacity>::push_back(const T &value)
    {
        if (count >= Capacity)
        {
            handle_overflow();
            return false;
        }
        new (static_cast<void *>(get_ptr(count))) T(value);
        ++count;
        return true;
    }
    template <typename T, size_t Capacity>
    inline bool StaticVector<T, Capacity>::push_back(T &&value)
    {
        if (count >= Capacity)
        {
            handle_overflow();
            return false;
        }
        new (static_cast<void *>(get_ptr(count))) T(static_cast<T &&>(value));
        ++count;
        return true;
    }
    template <typename T, size_t Capacity>
    inline bool StaticVector<T, Capacity>::erase(size_t index)
    {
        if (index >= count)
            return false;
#ifdef STATIC_VECTOR_CLEANUP_ENABLED
        get_ptr(index)->~T();
#endif
        // Shift subsequent elements left
        for (size_t i = index; i < count - 1; ++i)
        {
            *get_ptr(i) = static_cast<T &&>(*get_ptr(i + 1));
        }
#ifdef STATIC_VECTOR_CLEANUP_ENABLED
        get_ptr(count - 1)->~T();
#endif
        --count;
        return true;
    }
    template <typename T, size_t Capacity>
    inline void StaticVector<T, Capacity>::clear()
    {
#ifdef STATIC_VECTOR_CLEANUP_ENABLED
        for (size_t i = 0; i < count; ++i)
        {
            get_ptr(i)->~T();
        }
#endif
        count = 0;
    }
    template <typename T, size_t Capacity>
    inline T &StaticVector<T, Capacity>::operator[](size_t index)
    {
        return *get_ptr(index);
    }
    template <typename T, size_t Capacity>
    inline const T &StaticVector<T, Capacity>::operator[](size_t index) const
    {
        return *get_ptr(index);
    }
    template <typename T, size_t Capacity>
    inline void StaticVector<T, Capacity>::handle_overflow()
    {
#ifdef STATIC_VECTOR_OVERFLOW_ABORT
        EMBEDDED_ASSERT_FAIL(); 
#endif
    }
}