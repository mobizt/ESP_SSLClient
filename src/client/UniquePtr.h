/*
 * SPDX-FileCopyrightText: 2025 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace ReadyUtils {

  template <typename T>
  struct default_delete {
    void operator()(T* ptr) const {
      delete ptr;
    }
  };

  template <typename T, typename D = default_delete<T>>
  class unique_ptr {
  private:
    T* ptr;
    D deleter;

  public:

    explicit unique_ptr(T* p = nullptr) : ptr(p) {}

    ~unique_ptr() {
      if (ptr) deleter(ptr);
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

    unique_ptr(unique_ptr&& other) noexcept : ptr(other.ptr) {
      other.ptr = nullptr;
    }

    unique_ptr& operator=(unique_ptr&& other) noexcept {
      if (this != &other) {
        if (ptr) deleter(ptr); // Clean up current
        ptr = other.ptr;       // Take new
        other.ptr = nullptr;   // Nullify source
      }
      return *this;
    }

    T* get() const { return ptr; }
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    
    explicit operator bool() const { return ptr != nullptr; }

    T* release() {
      T* temp = ptr;
      ptr = nullptr;
      return temp;
    }

    void reset(T* p = nullptr) {
      if (ptr) deleter(ptr);
      ptr = p;
    }
  };

  template<typename T, typename... Args>
  unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(static_cast<Args&&>(args)...));
  }
}