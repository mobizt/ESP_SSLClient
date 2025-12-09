/**
 * SPDX-FileCopyrightText: 2025 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef PAIR_H
#define PAIR_H

namespace ReadyUtils
{
    template <typename T1, typename T2>
    struct Pair
    {
        T1 first;
        T2 second;

        constexpr Pair() = default;

        constexpr Pair(const T1 &a, const T2 &b)
            : first(a), second(b) {}

        template <typename U1, typename U2>
        constexpr Pair(U1 &&a, U2 &&b)
            : first(static_cast<U1 &&>(a)), second(static_cast<U2 &&>(b)) {}
    };
}
#endif