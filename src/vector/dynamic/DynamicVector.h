/**
 * SPDX-FileCopyrightText: 2025 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef DYNAMIC_VECTOR_H
#define DYNAMIC_VECTOR_H

#include <stdlib.h>
#include <stddef.h> 
#include <string.h>
#include <new> // fix for new/delete operators
#include "../Abort.h"

namespace ReadyUtils
{

#define DYNAMIC_VECTOR_HEAP_ENABLED
#define DYNAMIC_VECTOR_DIAGNOSTICS_ENABLED

    template <typename T>
    class DynamicVector;

    template <typename T>
    void swap(DynamicVector<T> &a, DynamicVector<T> &b) noexcept;

    template <typename T>
    class DynamicVector
    {
    private:
        T *data = nullptr;
        size_t count = 0;
        size_t capacity = 0;

    public:
        explicit inline DynamicVector(size_t initial_capacity = 4);

        inline DynamicVector(const DynamicVector &other);
        inline DynamicVector &operator=(const DynamicVector &other);

        inline ~DynamicVector();

        inline size_t size() const;
        inline size_t get_capacity() const;
        inline bool check_and_reserve();
        inline bool push_back(const T &value);
        inline bool push_back(T &&value);
        inline bool emplace_back(T &&value);

        inline bool erase(size_t index);
        inline void clear();
        inline bool reserve(size_t new_capacity);

        inline T &operator[](size_t index);
        inline const T &operator[](size_t index) const;

        friend void swap<>(DynamicVector<T> &a, DynamicVector<T> &b) noexcept;
    };
}

#include "DynamicVector.hpp"

#endif // DYNAMIC_VECTOR_H
