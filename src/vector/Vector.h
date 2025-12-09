// Vector.h
#pragma once
#include "VectorConfig.h"
#include "static/StaticVector.h"
#include "dynamic/DynamicVector.h"


#ifdef USE_STATIC_VECTOR
template<typename T, size_t Capacity>
using Vector = ReadyUtils::StaticVector<T, Capacity>;
#else
template<typename T>
using Vector = ReadyUtils::DynamicVector<T>;
#endif
