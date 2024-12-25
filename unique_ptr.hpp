#ifndef UNIQUE_PTR_HPP
#define UNIQUE_PTR_HPP

#include "default_deleter.hpp"
#include <cstddef>
#include <type_traits>
#include <utility>

namespace MySTL {

template <class _Tp, class _Deleter = DefaultDeleter<_Tp>> struct unique_ptr {
private:
  _Tp *_M_p;
  [[no_unique_address]] _Deleter _M_deleter;

  template <class _Up, class _UDeleter> friend struct unique_ptr;

public:
  using element_type = _Tp;
  using element_pointer = _Tp *;
  using deleter_type = _Deleter;

  unique_ptr(std::nullptr_t = nullptr) noexcept : _M_p(nullptr) {}

  explicit unique_ptr(element_pointer p) noexcept : _M_p(p) {}

  template <class _Up, class _UDeleter>
    requires(std::convertible_to<_Up *, _Tp *>)
  unique_ptr(unique_ptr<_Up, _UDeleter> &&__that) noexcept
      : _M_p(__that._M_p) { // 从子类型_Up的智能指针转换到_Tp类型的智能指针
    __that._M_p = nullptr;
  }

  ~unique_ptr() noexcept {
    if (_M_p) {
      _M_deleter(_M_p);
    }
  }

  unique_ptr(unique_ptr const &__that) = delete;

  unique_ptr &operator=(unique_ptr const &__that) = delete;

  unique_ptr(unique_ptr &&__that) noexcept : _M_p(__that._M_p) {
    __that._M_p = nullptr;
  }

  unique_ptr &operator=(unique_ptr &&__that) noexcept {
    if (this != &__that) [[likely]] {
      if (_M_p) {
        _M_deleter(_M_p);
      }
      _M_p = std::exchange(__that._M_p, nullptr);
    }
    return *this;
  }

  void swap(unique_ptr &__that) noexcept { std::swap(_M_p, __that._M_p); }

  _Tp *get() const noexcept { return _M_p; }

  _Tp *operator->() const noexcept { return _M_p; }

  std::add_lvalue_reference_t<_Tp> operator*() const noexcept { return *_M_p; }

  _Deleter get_deleter() const noexcept { return _M_deleter; }

  _Tp *release() noexcept {
    _Tp *__p = _M_p;
    _M_p = nullptr;
    return __p;
  }

  void reset(_Tp *__p = nullptr) noexcept {
    if (_M_p) {
      _M_deleter(_M_p);
    }
    _M_p = __p;
  }

  explicit operator bool() const noexcept { return _M_p != nullptr; }

  bool operator==(unique_ptr const &__that) const noexcept {
    return _M_p == __that._M_p;
  }

  bool operator!=(unique_ptr const &__that) const noexcept {
    return _M_p != __that._M_p;
  }

  bool operator<(unique_ptr const &__that) const noexcept {
    return _M_p < __that._M_p;
  }

  bool operator<=(unique_ptr const &__that) const noexcept {
    return _M_p <= __that._M_p;
  }

  bool operator>(unique_ptr const &__that) const noexcept {
    return _M_p > __that._M_p;
  }

  bool operator>=(unique_ptr const &__that) const noexcept {
    return _M_p >= __that._M_p;
  }
};

template <class _Tp, class _Deleter>
struct unique_ptr<_Tp[], _Deleter> : unique_ptr<_Tp, _Deleter> {
  using unique_ptr<_Tp, _Deleter>::unique_ptr; // 继承构造函数

  _Tp &operator[](std::size_t __i) const noexcept { return this->get()[__i]; }
};

template <class _Tp, class... _Args>
  requires(!std::is_unbounded_array_v<_Tp>)
auto make_unique(_Args &&...__args) -> unique_ptr<_Tp> {
  return unique_ptr<_Tp>(new _Tp(std::forward<_Args>(__args)...));
}

template <class _Tp>
  requires(!std::is_unbounded_array_v<_Tp>)
auto make_unique_for_overwrite() -> unique_ptr<_Tp> {
  return unique_ptr<_Tp>(new _Tp);
}

template <class _Tp>
  requires(std::is_unbounded_array_v<_Tp>)
auto make_unique(std::size_t __len) -> unique_ptr<_Tp> {
  return unique_ptr<_Tp>(new std::remove_extent_t<_Tp>[__len]());
}

template <class _Tp>
  requires(std::is_unbounded_array_v<_Tp>)
auto make_unique_for_overwrite(std::size_t __len) -> unique_ptr<_Tp> {
  return unique_ptr<_Tp>(new std::remove_extent_t<_Tp>[__len]);
}

} // namespace MySTL

#endif
