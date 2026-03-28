#pragma once

#include <cassert>
#include <new>
#include <type_traits>
#include <utility>

template <typename T>
class myoptional {
private:
    bool engaged_;
    alignas(T) unsigned char storage_[sizeof(T)];

    T* ptr() noexcept {
        return reinterpret_cast<T*>(storage_);
    }

    const T* ptr() const noexcept {
        return reinterpret_cast<const T*>(storage_);
    }

    void destroy() noexcept {
        if (engaged_) {
            ptr()->~T();
            engaged_ = false;
        }
    }

public:
    myoptional() noexcept : engaged_(false) {}

    myoptional(const T& value) : engaged_(true) {
        new (ptr()) T(value);
    }

    myoptional(T&& value) : engaged_(true) {
        new (ptr()) T(std::move(value));
    }

    myoptional(const myoptional& other) : engaged_(other.engaged_) {
        if (other.engaged_) {
            new (ptr()) T(*other.ptr());
        }
    }

    myoptional(myoptional&& other) noexcept(std::is_nothrow_move_constructible<T>::value)
        : engaged_(other.engaged_) {
        if (other.engaged_) {
            new (ptr()) T(std::move(*other.ptr()));
        }
    }

    ~myoptional() {
        destroy();
    }

    myoptional& operator=(const myoptional& other) {
        if (this != &other) {
            if (engaged_ && other.engaged_) {
                *ptr() = *other.ptr();
            } else if (engaged_) {
                destroy();
            } else if (other.engaged_) {
                new (ptr()) T(*other.ptr());
                engaged_ = true;
            }
            engaged_ = other.engaged_;
        }
        return *this;
    }

    myoptional& operator=(myoptional&& other) noexcept(
        std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_assignable<T>::value) {
        if (this != &other) {
            if (engaged_ && other.engaged_) {
                *ptr() = std::move(*other.ptr());
            } else if (engaged_) {
                destroy();
            } else if (other.engaged_) {
                new (ptr()) T(std::move(*other.ptr()));
                engaged_ = true;
            }
            engaged_ = other.engaged_;
        }
        return *this;
    }

    explicit operator bool() const noexcept {
        return engaged_;
    }

    bool has_value() const noexcept {
        return engaged_;
    }

    T& operator*() {
        assert(engaged_);
        return *ptr();
    }

    const T& operator*() const {
        assert(engaged_);
        return *ptr();
    }

    T* operator->() {
        assert(engaged_);
        return ptr();
    }

    const T* operator->() const {
        assert(engaged_);
        return ptr();
    }

    T& value() {
        assert(engaged_);
        return *ptr();
    }

    const T& value() const {
        assert(engaged_);
        return *ptr();
    }

    template <typename... Args>
    T& emplace(Args&&... args) {
        destroy();
        new (ptr()) T(std::forward<Args>(args)...);
        engaged_ = true;
        return *ptr();
    }

    void reset() noexcept {
        destroy();
    }

    T value_or(T default_value) const {
        return engaged_ ? *ptr() : default_value;
    }
};
