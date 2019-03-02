//
// Created by qrort on 2/16/19.
//

#ifndef LINKED_PTR_LINKED_PTR_HPP
#define LINKED_PTR_LINKED_PTR_HPP
#include <cassert>
#include <type_traits>
#include <algorithm>

namespace smart_ptr{

    namespace details{

        class linked_ptr_base{
        public:
            linked_ptr_base *l = nullptr;
            linked_ptr_base *r = nullptr;
            linked_ptr_base() = default;
        protected:
            bool unique() const noexcept {
                return (l == nullptr && r == nullptr);
            }
            void insert(linked_ptr_base* other) noexcept {
                assert(other);
                this->l = other;
                if (other->r) {
                    this->r = other->r;
                    other->r->l = this;
                }
                other->r = this;
            }
            void erase() noexcept {
                if (l) {
                    l->r = r;

                }
                if (r) {
                    r->l = l;
                }
                l = r = nullptr;
            }
        };

    }


    template <typename T>
    class linked_ptr;

    template <typename T>
    class linked_ptr : public details::linked_ptr_base {
    private:
        template <typename U> friend class linked_ptr;
        template <typename U>
        using _Convertible = std::enable_if<std::is_convertible<U*, T*>::value>;
        using _Base = details::linked_ptr_base;

    public:
        using element_type = T;

        template <typename U>
        friend void swap(linked_ptr<U>& a, linked_ptr<U>& b) noexcept;

        void swap(linked_ptr<T>& other) noexcept {
            if (l) {
                l->r = &other;
            }
            if (r) {
                r->l = &other;
            }
            if (other.l) {
                other.l->r = this;
            }
            if (other.r) {
                other.r->l = this;
            }
            std::swap(l, other.l);
            std::swap(r, other.r);
            std::swap(ptr, other.ptr);
        }

        explicit linked_ptr() noexcept = default;
        explicit linked_ptr(std::nullptr_t) noexcept : linked_ptr() {}
        linked_ptr(T* _ptr) noexcept : ptr(_ptr) {}
        template <typename U, typename = _Convertible<U>>
        linked_ptr(U* _ptr) noexcept : ptr(static_cast<T*>(_ptr)) {}
        linked_ptr(const linked_ptr<T>& other) noexcept : ptr(other.get()) {
            insert(const_cast<linked_ptr<T>*>(&other));
        }
        template <typename U, typename = _Convertible<U>>
        linked_ptr(const linked_ptr<U>& other) noexcept : ptr(other.get()) {
            insert(const_cast<linked_ptr<U>*>(&other));
        }
        linked_ptr(linked_ptr<T>&& other) noexcept {
            this->swap(other);
        }

        ~linked_ptr() {
            reset();
        }

        template <typename U, typename = _Convertible<U>>
        linked_ptr& operator =(const linked_ptr<U>& other) noexcept {
            reset(other.get());
            insert(other);
            return *this;
        }

        void reset() noexcept {
            if (unique()) {
                delete ptr;
            } else {
                erase();
            }
            ptr = nullptr;
        }
        void reset(T* _ptr) noexcept {
            reset();
            ptr = _ptr;
        }
        template <typename U, typename = _Convertible<U>>
        void reset(U* _ptr) noexcept {
            reset();
            ptr = _ptr;
        }


        T* get() const noexcept {
            return ptr;
        }

        bool unique() const noexcept {
            if (ptr == nullptr) return false;
            return _Base::unique();
        }

        T& operator*() const noexcept {
            return *ptr;
        }

        T* operator->() const noexcept {
            return ptr;
        }

        explicit operator bool() const noexcept {
            return ptr != nullptr;
        }

    private:
        T* ptr = nullptr;
    };

    template <typename U>
    void swap(linked_ptr<U>& a, linked_ptr<U>& b) noexcept {
        a.swap(b);
    }

    template <typename U, typename V>
    bool operator == (const linked_ptr<U>& a, const linked_ptr<V>& b) {
        return (a.get() == b.get());
    }
    template <typename U, typename V>
    bool operator != (const linked_ptr<U>& a, const linked_ptr<V>& b) {
        return !(a == b);
    }
    template <typename U, typename V>
    bool operator < (const linked_ptr<U>& a, const linked_ptr<V>& b) {
        using UT = typename linked_ptr<U>::element_type;
        using VT = typename linked_ptr<V>::element_type;
        using CT = typename std::common_type<UT, VT>::type;
        return std::less<CT*>()(a.get(), b.get());
    }
    template <typename U, typename V>
    bool operator > (const linked_ptr<U>& a, const linked_ptr<V>& b) {
        return (b < a);
    }
    template <typename U, typename V>
    bool operator <= (const linked_ptr<U>& a, const linked_ptr<V>& b) {
        return !(b < a);
    }
    template <typename U, typename V>
    bool operator >= (const linked_ptr<U>& a, const linked_ptr<V>& b) {
        return !(a < b);
    }
    template <typename T>
    bool operator==( const linked_ptr<T>& a, std::nullptr_t b ) noexcept {
        return (!a);
    }
    template<typename T>
    bool operator==( std::nullptr_t a, const linked_ptr<T>& b ) noexcept {
        return (!b);
    }
    template<typename T>
    bool operator!=( const linked_ptr<T>& a, std::nullptr_t b ) noexcept {
        return (bool)a;
    }
    template<typename T>
    bool operator!=( std::nullptr_t a, const linked_ptr<T>& b ) noexcept {
        return (bool)b;
    }
    template<typename T>
    bool operator<( const linked_ptr<T>& a, std::nullptr_t b ) noexcept {
        return std::less<typename linked_ptr<T>::element_type>()(a.get(), nullptr);
    }
    template<typename T>
    bool operator<( std::nullptr_t a, const linked_ptr<T>& b ) noexcept {
        return std::less<typename linked_ptr<T>::element_type>()(nullptr, b.get());
    }
    template<typename T>
    bool operator>( const linked_ptr<T>& a, std::nullptr_t b ) noexcept {
        return (nullptr < a);
    }
    template<typename T>
    bool operator>( std::nullptr_t a, const linked_ptr<T>& b ) noexcept {
        return (b < nullptr);
    }
    template<typename T>
    bool operator<=( const linked_ptr<T>& a, std::nullptr_t b ) noexcept {
        return !(nullptr < a);
    }
    template<typename T>
    bool operator<=( std::nullptr_t a, const linked_ptr<T>& b ) noexcept {
        return !(b < nullptr);
    }
    template<typename T>
    bool operator>=( const linked_ptr<T>& a, std::nullptr_t b ) noexcept {
        return !(a < nullptr);
    }
    template<typename T>
    bool operator>=( std::nullptr_t a, const linked_ptr<T>& b ) noexcept {
        return !(nullptr < b);
    }

}
#endif //LINKED_PTR_LINKED_PTR_HPP
