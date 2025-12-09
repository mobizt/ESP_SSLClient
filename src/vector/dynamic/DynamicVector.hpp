/**
 * SPDX-FileCopyrightText: 2025 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace ReadyUtils
{
    template <typename T>
    void swap(DynamicVector<T> &a, DynamicVector<T> &b) noexcept
    {
        // C-style pointer/primitive swapping
        T *temp_data = a.data;
        a.data = b.data;
        b.data = temp_data;

        size_t temp_count = a.count;
        a.count = b.count;
        b.count = temp_count;

        size_t temp_capacity = a.capacity;
        a.capacity = b.capacity;
        b.capacity = temp_capacity;
    }

    template <typename T>
    inline DynamicVector<T>::DynamicVector(size_t initial_capacity)
        : capacity(initial_capacity)
    {
#ifdef DYNAMIC_VECTOR_HEAP_ENABLED
        data = static_cast<T *>(malloc(sizeof(T) * capacity));
#endif
    }

    template <typename T>
    inline DynamicVector<T>::DynamicVector(const DynamicVector &other)
        : count(other.count), capacity(other.capacity)
    {
#ifdef DYNAMIC_VECTOR_HEAP_ENABLED
        data = static_cast<T *>(malloc(sizeof(T) * capacity));
        if (data)
        {
            for (size_t i = 0; i < count; ++i)
                new (&data[i]) T(other.data[i]);
        }
        else
        {
            count = 0;
            capacity = 0;
        }
#endif
    }

    template <typename T>
    inline DynamicVector<T> &DynamicVector<T>::operator=(const DynamicVector &other)
    {
        // Create a temporary copy (this uses the Copy Constructor above)
        DynamicVector<T> temp(other);

        // Swap the state of the current object with the temporary copy (uses custom swap)
        // We use unqualified swap() to allow Argument-Dependent Lookup (ADL) to find our custom swap.
        swap(*this, temp);

        // When 'temp' goes out of scope, it destroys the old data of '*this'.
        return *this;
    }

    template <typename T>
    inline DynamicVector<T>::~DynamicVector()
    {
        clear();
#ifdef DYNAMIC_VECTOR_HEAP_ENABLED
        free(data);
#endif
    }

    template <typename T>
    inline size_t DynamicVector<T>::size() const { return count; }

    template <typename T>
    inline size_t DynamicVector<T>::get_capacity() const { return capacity; }

    template <typename T>
    inline bool DynamicVector<T>::check_and_reserve()
    {
        if (count < capacity)
        {
            return true;
        }
#ifdef DYNAMIC_VECTOR_HEAP_ENABLED
        if (!reserve(capacity == 0 ? 4 : capacity * 2))
        {
#ifdef DYNAMIC_VECTOR_DIAGNOSTICS_ENABLED
EMBEDDED_ASSERT_FAIL();
#endif
            return false;
        }
#else
#ifdef DYNAMIC_VECTOR_DIAGNOSTICS_ENABLED
EMBEDDED_ASSERT_FAIL();
#endif
        return false;
#endif
        return true;
    }

    template <typename T>
    inline bool DynamicVector<T>::push_back(const T &value)
    {
        if (!check_and_reserve())
            return false;

        new (&data[count]) T(value);
        ++count;
        return true;
    }

    template <typename T>
    inline bool DynamicVector<T>::push_back(T &&value)
    {
        if (!check_and_reserve())
            return false;

        new (&data[count]) T(static_cast<T &&>(value));
        ++count;
        return true;
    }

    template <typename T>
    inline bool DynamicVector<T>::emplace_back(T &&value)
    {
        return push_back(static_cast<T &&>(value));
    }

    template <typename T>
    inline bool DynamicVector<T>::erase(size_t index)
    {
        if (index >= count)
            return false;

        data[index].~T();

        if (index < count - 1)
        {
            for (size_t i = index; i < count - 1; ++i)
            {
                data[i] = static_cast<T &&>(data[i + 1]);
            }

            // After the loop, the last object's *storage* (data[count - 1]) now holds
            // a valid copy of the *second*-to-last element. We must explicitly
            // destroy the ghost element in the last slot, which is no longer logically
            // part of the vector, to avoid calling its destructor later.
            data[count - 1].~T();
        }

        --count;
        return true;
    }

    template <typename T>
    inline void DynamicVector<T>::clear()
    {
        for (size_t i = 0; i < count; ++i)
        {
            data[i].~T();
        }
        count = 0;
    }

    template <typename T>
    inline bool DynamicVector<T>::reserve(size_t new_capacity)
    {
#ifdef DYNAMIC_VECTOR_HEAP_ENABLED
        if (new_capacity <= capacity)
            return true;

        T *new_data = static_cast<T *>(malloc(sizeof(T) * new_capacity));
        if (!new_data)
            return false;

        for (size_t i = 0; i < count; ++i)
        {
            new (&new_data[i]) T(static_cast<T &&>(data[i])); // Move construct
            data[i].~T();                                     // Destroy old
        }

        free(data);
        data = new_data;
        capacity = new_capacity;
        return true;
#else
        return false;
#endif
    }

    template <typename T>
    inline T &DynamicVector<T>::operator[](size_t index) { return data[index]; }

    template <typename T>
    inline const T &DynamicVector<T>::operator[](size_t index) const { return data[index]; }

}
