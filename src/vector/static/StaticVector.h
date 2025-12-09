/**
 * SPDX-FileCopyrightText: 2025 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef STATIC_VECTOR_H
#define STATIC_VECTOR_H
#include <Arduino.h>
#include "../Abort.h"
namespace ReadyUtils
{

#define STATIC_VECTOR_CLEANUP_ENABLED
#define STATIC_VECTOR_OVERFLOW_ABORT

    template <typename T, size_t Capacity>
    class StaticVector
    {
        static_assert(Capacity > 0, "StaticVector capacity must be > 0");

    public:
        alignas(T) char data_storage[sizeof(T) * Capacity];
        size_t count = 0;

        T *get_ptr(size_t i);
        const T *get_ptr(size_t i) const;

        inline StaticVector();
        inline ~StaticVector();

        inline StaticVector(const StaticVector &other);
        inline StaticVector &operator=(const StaticVector &other);

        inline constexpr size_t size() const;

        inline bool push_back(const T &value);
        inline bool push_back(T &&value);
        inline bool erase(size_t index);
        inline void clear();

        inline T &operator[](size_t index);
        inline const T &operator[](size_t index) const;

    private:
        void handle_overflow();
    };
}

#include "StaticVector.hpp"

#endif // STATIC_VECTOR_H
