#pragma once

#include <algorithm>
#include <cassert>
#include <new>
#include <type_traits>
#include <utility>

namespace detail {
    template <typename T, typename... Ts>
    struct index_of;

    template <typename T, typename First, typename... Rest>
    struct index_of<T, First, Rest...> {
        static constexpr std::size_t value = std::is_same<T, First>::value ? 0 : 1 + index_of<T, Rest...>::value;
    };

    template <typename T>
    struct index_of<T> {
        static constexpr std::size_t value = static_cast<std::size_t>(-1);
    };

    template <std::size_t I, typename... Ts>
    struct type_at;

    template <std::size_t I, typename First, typename... Rest>
    struct type_at<I, First, Rest...> {
        using type = typename type_at<I - 1, Rest...>::type;
    };

    template <typename First, typename... Rest>
    struct type_at<0, First, Rest...> {
        using type = First;
    };
}

template <typename... Ts>
class myvariant {
private:
    static_assert(sizeof...(Ts) > 0, "myvariant must contain at least one type");

    static constexpr std::size_t type_count = sizeof...(Ts);
    static constexpr std::size_t data_size = std::max({sizeof(Ts)...});
    static constexpr std::size_t data_align = std::max({alignof(Ts)...});
    using storage_t = std::aligned_storage_t<data_size, data_align>;

    storage_t data_;
    std::size_t index_;

    template <std::size_t I>
    using alternative_t = typename detail::type_at<I, Ts...>::type;

    template <std::size_t I>
    alternative_t<I>& get_storage() {
        return *reinterpret_cast<alternative_t<I>*>(&data_);
    }

    template <std::size_t I>
    const alternative_t<I>& get_storage() const {
        return *reinterpret_cast<const alternative_t<I>*>(&data_);
    }

    void destroy() noexcept {
        destroy_impl(0);
    }

    void destroy_impl(std::size_t i) noexcept {
        if (i >= type_count) {
            return;
        }
        if (i == 0) {
            reinterpret_cast<alternative_t<0>*>(&data_)->~alternative_t<0>();
        } else {
            destroy_impl(i - 1);
        }
    }

    template <std::size_t I = 0>
    void destroy_impl() noexcept {
        if constexpr (I < type_count) {
            if (index_ == I) {
                get_storage<I>().~alternative_t<I>();
            } else {
                destroy_impl<I + 1>();
            }
        }
    }

    template <std::size_t I = 0>
    void copy_construct(const myvariant& other) {
        if constexpr (I < type_count) {
            if (other.index_ == I) {
                new (&data_) alternative_t<I>(other.template get<I>());
            } else {
                copy_construct<I + 1>(other);
            }
        }
    }

    template <std::size_t I = 0>
    void move_construct(myvariant&& other) {
        if constexpr (I < type_count) {
            if (other.index_ == I) {
                new (&data_) alternative_t<I>(std::move(other.template get<I>()));
            } else {
                move_construct<I + 1>(std::move(other));
            }
        }
    }

    template <std::size_t I = 0>
    void assign_same(const myvariant& other) {
        if constexpr (I < type_count) {
            if (index_ == I) {
                get_storage<I>() = other.template get<I>();
            } else {
                assign_same<I + 1>(other);
            }
        }
    }

    template <std::size_t I = 0>
    void assign_same(myvariant&& other) {
        if constexpr (I < type_count) {
            if (index_ == I) {
                get_storage<I>() = std::move(other.template get<I>());
            } else {
                assign_same<I + 1>(std::move(other));
            }
        }
    }

public:
    template <typename U,
              typename Decayed = std::decay_t<U>,
              typename = std::enable_if_t<!std::is_same<Decayed, myvariant>::value &&
                                          detail::index_of<Decayed, Ts...>::value != static_cast<std::size_t>(-1)>>
    myvariant(U&& value) noexcept(std::is_nothrow_constructible<Decayed, U&&>::value)
        : index_(detail::index_of<Decayed, Ts...>::value) {
        new (&data_) Decayed(std::forward<U>(value));
    }

    myvariant(const myvariant& other) : index_(other.index_) {
        copy_construct(other);
    }

    myvariant(myvariant&& other) noexcept(
        std::conjunction_v<std::is_nothrow_move_constructible<Ts>...>)
        : index_(other.index_) {
        move_construct(std::move(other));
    }

    ~myvariant() {
        destroy_impl();
    }

    myvariant& operator=(const myvariant& other) {
        if (this != &other) {
            if (index_ == other.index_) {
                assign_same(other);
            } else {
                destroy_impl();
                copy_construct(other);
                index_ = other.index_;
            }
        }
        return *this;
    }

    myvariant& operator=(myvariant&& other) noexcept(
        std::conjunction_v<std::is_nothrow_move_constructible<Ts>..., std::is_nothrow_move_assignable<Ts>...>) {
        if (this != &other) {
            if (index_ == other.index_) {
                assign_same(std::move(other));
            } else {
                destroy_impl();
                move_construct(std::move(other));
                index_ = other.index_;
            }
        }
        return *this;
    }

    template <typename U,
              typename Decayed = std::decay_t<U>,
              typename = std::enable_if_t<!std::is_same<Decayed, myvariant>::value &&
                                          detail::index_of<Decayed, Ts...>::value != static_cast<std::size_t>(-1)>>
    myvariant& operator=(U&& value) {
        myvariant temp(std::forward<U>(value));
        *this = std::move(temp);
        return *this;
    }

    std::size_t index() const noexcept {
        return index_;
    }

    template <std::size_t I>
    alternative_t<I>& get() {
        assert(index_ == I);
        return get_storage<I>();
    }

    template <std::size_t I>
    const alternative_t<I>& get() const {
        assert(index_ == I);
        return get_storage<I>();
    }

    template <typename U>
    U& get() {
        constexpr std::size_t I = detail::index_of<U, Ts...>::value;
        static_assert(I != static_cast<std::size_t>(-1), "Type not found in myvariant");
        return get<I>();
    }

    template <typename U>
    const U& get() const {
        constexpr std::size_t I = detail::index_of<U, Ts...>::value;
        static_assert(I != static_cast<std::size_t>(-1), "Type not found in myvariant");
        return get<I>();
    }

    template <typename U>
    bool holds_alternative() const noexcept {
        constexpr std::size_t I = detail::index_of<U, Ts...>::value;
        static_assert(I != static_cast<std::size_t>(-1), "Type not found in myvariant");
        return index_ == I;
    }
};
