#ifndef SHARED_PTR_HPP
#define SHARED_PTR_HPP
#include "default_deleter.hpp"
#include "unique_ptr.hpp"
#include <algorithm>
#include <atomic>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace MySTL {

struct _SpCounter {

  std::atomic<long> _M_refcnt;

  _SpCounter() noexcept : _M_refcnt(1){};

  _SpCounter(_SpCounter &&) = delete;

  void _M_incref() noexcept {
    _M_refcnt.fetch_add(1, std::memory_order_relaxed);
  }

  void _M_decref() noexcept {
    if (_M_refcnt.fetch_sub(1, std::memory_order_relaxed) == 1) {
      delete this;
    }
  }

  long _M_cntref() const noexcept {
    return _M_refcnt.load(std::memory_order_relaxed);
  }

  virtual ~_SpCounter() = default;
};

template <class _Tp, class _Deleter> struct _SpCounterImpl final : _SpCounter {

  _Tp *_M_ptr;

  [[no_unique_address]] _Deleter _M_deleter;

  explicit _SpCounterImpl(_Tp *__ptr) noexcept : _M_ptr(__ptr) {}

  explicit _SpCounterImpl(_Tp *__ptr, _Deleter __deleter) noexcept
      : _M_ptr(__ptr), _M_deleter(std::move(__deleter)) {}

  ~_SpCounterImpl() noexcept override { _M_deleter(_M_ptr); }
};

template <class _Tp, class _Deleter>
struct _SpCounterImplFused final : _SpCounter {
  _Tp *_M_ptr;
  void *_M_mem;
  [[no_unique_address]] _Deleter _M_deleter;

  explicit _SpCounterImplFused(_Tp *__ptr, void *__mem,
                               _Deleter __deleter) noexcept
      : _M_ptr(__ptr), _M_mem(__mem), _M_deleter(std::move(__deleter)) {}

  ~_SpCounterImplFused() noexcept { _M_deleter(_M_ptr); }

  void operator delete(void *__mem) noexcept {
#if __cpp_aligned_new
    ::operator delete(__mem, std::align_val_t(std::max(
                                 alignof(_Tp), alignof(_SpCounterImplFused))));
#else
    ::operator delete(__mem);
#endif
  }
};

template <class _Tp> struct shared_ptr {
private:
  _Tp *_M_ptr;
  _SpCounter *_M_owner;

  template <class> friend struct shared_ptr;

  explicit shared_ptr(_Tp *__ptr, _SpCounter *__owner) noexcept
      : _M_ptr(__ptr), _M_owner(__owner) {}

public:
  using element_type = _Tp;
  using element_pointer = _Tp *;

  shared_ptr(std::nullptr_t = nullptr) noexcept : _M_owner(nullptr) {}

  template <class _Yp>
    requires(std::is_convertible_v<_Yp *, _Tp *>)
  explicit shared_ptr(_Yp *__ptr)
      : _M_ptr(__ptr),
        _M_owner(new _SpCounterImpl<_Yp, DefaultDeleter<_Yp>>(__ptr)) {
    _S_setupEnableSharedFromThis(_M_ptr, _M_owner);
  }

  template <class _Yp, class _Deleter>
    requires(std::is_convertible_v<_Yp *, _Tp *>)
  explicit shared_ptr(_Yp *__ptr, _Deleter __deleter)
      : _M_ptr(__ptr), _M_owner(new _SpCounterImpl<_Yp, _Deleter>(
                           __ptr, std::move(__deleter))) {
    _S_setupEnableSharedFromThis(_M_ptr, _M_owner);
  }

  template <class _Yp, class _Deleter>
    requires(std::is_convertible_v<_Yp *, _Tp *>)
  explicit shared_ptr(unique_ptr<_Yp, _Deleter> &&__ptr)
      : shared_ptr(__ptr.release(), __ptr.get_deleter()) {}

  template <class _Yp>
  inline friend shared_ptr<_Yp>
  _S_makeSharedFused(_Yp *__ptr, _SpCounter *__owner) noexcept;

  shared_ptr(shared_ptr const &__that) noexcept
      : _M_ptr(__that._M_ptr), _M_owner(__that._M_owner) {
    if (_M_owner) {
      _M_owner->_M_incref();
    }
  }

  template <class _Yp>
    requires(std::is_convertible_v<_Yp *, _Tp *>)
  shared_ptr(shared_ptr<_Yp> const &__that) noexcept
      : _M_ptr(__that._M_ptr), _M_owner(__that._M_owner) {
    if (_M_owner) {
      _M_owner->_M_incref();
    }
  }

  shared_ptr(shared_ptr &&__that) noexcept
      : _M_ptr(__that._M_ptr), _M_owner(__that._M_owner) {
    __that._M_ptr = nullptr;
    __that._M_owner = nullptr;
  }

  template <class _Yp>
    requires(std::is_convertible_v<_Yp *, _Tp *>)
  shared_ptr(shared_ptr<_Yp> &&__that) noexcept
      : _M_ptr(__that._M_ptr), _M_owner(__that._M_owner) {
    __that._M_ptr = nullptr;
    __that._M_owner = nullptr;
  }

  template <class _Yp>
  shared_ptr(shared_ptr<_Yp> const &__that, _Tp *__ptr) noexcept
      : _M_ptr(__ptr), _M_owner(__that._M_owner) {
    if (_M_owner) {
      _M_owner->_M_incref();
    }
  }

  template <class _Yp>
  shared_ptr(shared_ptr<_Yp> const &&__that, _Tp *__ptr) noexcept
      : _M_ptr(__ptr), _M_owner(__that._M_owner) {
    __that._M_ptr = nullptr;
    __that._M_owner = nullptr;
  }

  auto operator=(shared_ptr const &__that) noexcept -> shared_ptr & {
    if (this == &__that) {
      return *this;
    }
    if (this->_M_owner) {
      _M_owner->_M_decref();
    }
    _M_ptr = __that._M_ptr;
    _M_owner = __that._M_owner;

    if (this->_M_owner) {
      _M_owner->_M_incref();
    }
    return *this;
  }

  auto operator=(shared_ptr &&__that) noexcept -> shared_ptr & {
    if (this == &__that) {
      return *this;
    }
    if (this->_M_owner) {
      _M_owner->_M_decref();
    }

    _M_ptr = __that._M_ptr;
    _M_owner = __that._M_owner;
    __that._M_ptr = nullptr;
    __that._M_owner = nullptr;
    return *this;
  }

  template <class _Yp>
    requires(std::is_convertible_v<_Yp *, _Tp *>)
  auto operator=(shared_ptr<_Yp> const &__that) noexcept -> shared_ptr & {
    if (this == &__that) {
      return *this;
    }
    if (this->_M_owner) {
      _M_owner->_M_decref();
    }
    _M_ptr = __that._M_ptr;
    _M_owner = __that._M_owner;

    if (this->_M_owner) {
      _M_owner->_M_incref();
    }
    return *this;
  }

  template <class _Yp>
    requires(std::is_convertible_v<_Yp *, _Tp *>)
  auto operator=(shared_ptr<_Yp> &&__that) noexcept -> shared_ptr & {
    if (this == &__that) {
      return *this;
    }
    if (this->_M_owner) {
      _M_owner->_M_decref();
    }
    _M_ptr = __that._M_ptr;
    _M_owner = __that._M_owner;
    __that._M_ptr = nullptr;
    __that._M_owner = nullptr;
    return *this;
  }

  void reset() noexcept {
    if (_M_owner) {
      _M_owner->_M_decref();
    }
    _M_owner = nullptr;
    _M_ptr = nullptr;
  }

  template <class _Yp> void reset(_Yp *__ptr) {
    if (_M_owner) {
      _M_owner->_M_decref();
    }
    _M_ptr = nullptr;
    _M_owner = nullptr;
    _M_ptr = __ptr;
    _M_owner = new _SpCounterImpl<_Yp, DefaultDeleter<_Yp>>(__ptr);
    _S_setupEnableSharedFromThis(_M_ptr, _M_owner);
  }

  template <class _Yp, class _Deleter>
  void reset(_Yp *__ptr, _Deleter __deleter) {
    if (_M_owner) {
      _M_owner->_M_decref();
    }
    _M_ptr = nullptr;
    _M_owner = nullptr;
    _M_ptr = __ptr;
    _M_owner = new _SpCounterImpl<_Yp, _Deleter>(__ptr, std::move(__deleter));
    _S_setupEnableSharedFromThis(_M_ptr, _M_owner);
  }

  ~shared_ptr() noexcept {
    if (_M_owner) {
      _M_owner->_M_decref();
    }
  }

  auto use_count() noexcept -> long {
    return _M_owner ? _M_owner->_M_cntref() : 0;
  }

  auto unique() noexcept -> bool {
    return _M_owner ? _M_owner->_M_cntref() == 1 : true;
  }

  template <class _Yp>
  auto operator==(shared_ptr<_Yp> const &__that) const noexcept -> bool {
    return _M_ptr == __that._M_ptr;
  }

  template <class _Yp>
  auto operator!=(shared_ptr<_Yp> const &__that) const noexcept -> bool {
    return _M_ptr != __that._M_ptr;
  }

  template <class _Yp>
  auto operator<(shared_ptr<_Yp> const &__that) const noexcept -> bool {
    return _M_ptr < __that._M_ptr;
  }

  template <class _Yp>
  auto operator<=(shared_ptr<_Yp> const &__that) const noexcept -> bool {
    return _M_ptr <= __that._M_ptr;
  }

  template <class _Yp>
  auto operator>(shared_ptr<_Yp> const &__that) const noexcept -> bool {
    return _M_ptr > __that._M_ptr;
  }

  template <class _Yp>
  auto operator>=(shared_ptr<_Yp> const &__that) const noexcept -> bool {
    return _M_ptr >= __that._M_ptr;
  }

  template <class _Yp>
  auto owner_before(shared_ptr<_Yp> const &__that) const noexcept -> bool {
    return _M_owner < __that._M_owner;
  }

  template <class _Yp>
  auto owner_equal(shared_ptr<_Yp> const &__that) const noexcept -> bool {
    return _M_owner == __that._M_owner;
  }

  void swap(shared_ptr &__that) noexcept {
    std::swap(_M_ptr, __that._M_ptr);
    std::swap(_M_owner, __that._M_owner);
  }

  auto get() const noexcept -> _Tp * { return _M_ptr; }

  auto operator->() const noexcept -> _Tp * { return _M_ptr; }

  auto operator*() const noexcept -> std::add_lvalue_reference_t<_Tp> {
    return *_M_ptr;
  }

  explicit operator bool() const noexcept { return _M_ptr != nullptr; }
};

template <class _Tp>
inline auto _S_makeSharedFused(_Tp *__ptr, _SpCounter *__owner) noexcept
    -> shared_ptr<_Tp> {
  return shared_ptr<_Tp>(__ptr, __owner);
}

template <class _Tp> struct shared_ptr<_Tp[]> : shared_ptr<_Tp> {
  using shared_ptr<_Tp>::shared_ptr;

  auto operator[](std::size_t __i) -> std::add_lvalue_reference<_Tp> {
    return this->get()[__i];
  }
};

template <class _Tp> struct enable_shared_from_this {
private:
  _SpCounter *_M_owner;

protected:
  enable_shared_from_this() noexcept : _M_owner(nullptr){};

  auto shared_from_this() -> shared_ptr<_Tp> {
    static_assert(std::is_base_of_v<enable_shared_from_this, _Tp>,
                  "must be derived class");
    if (!_M_owner) {
      throw std::bad_weak_ptr();
    }
    _M_owner->_M_incref();
    return _S_makeSharedFused(static_cast<_Tp *>(this), _M_owner);
  }
  auto shared_from_this() const -> shared_ptr<_Tp const> {
    static_assert(std::is_base_of_v<enable_shared_from_this, _Tp>,
                  "must be derived class");
    if (!_M_owner) {
      throw std::bad_weak_ptr();
    }
    _M_owner->_M_incref();
    return _S_makeSharedFused(static_cast<_Tp const *>(this), _M_owner);
  }

  template <class _Up>
  inline friend void
  _S_setupEnableSharedFromThisOwner(enable_shared_from_this<_Up> *,
                                    _SpCounter *);
};

template <class _Up>
inline void
_S_setupEnableSharedFromThisOwner(enable_shared_from_this<_Up> *__ptr,
                                  _SpCounter *__owner) {
  __ptr->_M_owner = __owner;
}

template <class _Tp>
  requires(std::is_base_of_v<enable_shared_from_this<_Tp>, _Tp>)
void _S_setupEnableSharedFromThis(_Tp *__ptr, _SpCounter *__owner) {
  _S_setupEnableSharedFromThisOwner(
      static_cast<enable_shared_from_this<_Tp> *>(__ptr), __owner);
}

template <class _Tp>
  requires(!std::is_base_of_v<enable_shared_from_this<_Tp>, _Tp>)
void _S_setupEnableSharedFromThis(_Tp *, _SpCounter *) {}

template <class _Tp, class... _Args>
  requires(!std::is_unbounded_array_v<_Tp>)
auto make_shared(_Args &&...__args) -> shared_ptr<_Tp> {
  auto const __deleter = [](_Tp *__ptr) noexcept { __ptr->~_Tp(); };
  using _Counter = _SpCounterImplFused<_Tp, decltype(__deleter)>;
  constexpr std::size_t __offset = std::max(alignof(_Tp), sizeof(_Counter));
  constexpr std::size_t __align = std::max(alignof(_Tp), alignof(_Counter));
  constexpr std::size_t __size = __offset + sizeof(_Tp);
#if __cpp_aligned_new
  void *__mem = ::operator new(__size, std::align_val_t(__align));
  _Counter *__counter = reinterpret_cast<_Counter *>(__mem);
#else
  void *__mem = ::operator new(__size + __align);
  _Counter *__counter =
      reinterpret_cast<_SpC *>(reinterpret_cast<std::size_t>(__mem) & __align);
#endif
  _Tp *__object =
      reinterpret_cast<_Tp *>(reinterpret_cast<char *>(__counter) + __offset);
  try {
    new (__object) _Tp(std::forward<_Args>(__args)...);
  } catch (...) {
#if __cpp_aligned_new
    ::operator delete(__mem, std::align_val_t(__align));
#else
    ::operator delete(__mem);
#endif
    throw;
  }
  new (__counter) _Counter(__object, __mem, __deleter);
  _S_setupEnableSharedFromThis(__object, __counter);
  return _S_makeSharedFused(__object, __counter);
}
template <class _Tp>
  requires(!std::is_unbounded_array_v<_Tp>)
auto make_shared_for_over_write() -> shared_ptr<_Tp> {
  auto const __deleter = [](_Tp *__ptr) noexcept { __ptr->~_Tp(); };
  using _Counter = _SpCounterImplFused<_Tp, decltype(__deleter)>;
  constexpr std::size_t __offset = std::max(alignof(_Tp), sizeof(_Counter));
  constexpr std::size_t __align = std::max(alignof(_Tp), alignof(_Counter));
  constexpr std::size_t __size = __offset + sizeof(_Tp);
#if __cpp_aligned_new
  void *__mem = ::operator new(__size, std::align_val_t(__align));
  _Counter *__counter = reinterpret_cast<_Counter *>(__mem);
#else
  void *__mem = ::operator new(__size + __align);
  _SpC *__counter =
      reinterpret_cast<_SpC *>(reinterpret_cast<std::size_t>(__mem) & __align);
#endif
  _Tp *__object =
      reinterpret_cast<_Tp *>(reinterpret_cast<char *>(__counter) + __offset);
  try {
    new (__object) _Tp;
  } catch (...) {
#if __cpp_aligned_new
    ::operator delete(__mem, std::align_val_t(__align));
#else
    ::operator delete(__mem);
#endif
    throw;
  }
  new (__counter) _Counter(__object, __mem, __deleter);
  _S_setupEnableSharedFromThis(__object, __counter);
  return _S_makeSharedFused(__object, __counter);
}

template <class _Tp, class... _Args>
  requires(std::is_unbounded_array_v<_Tp>)
auto make_shared(std::size_t __len) -> shared_ptr<_Tp> {
  std::remove_extent_t<_Tp> *__p = new std::remove_extent_t<_Tp>[__len];
  try {
    return SharedPtr<_Tp>(__p);
  } catch (...) {
    delete[] __p;
    throw;
  }
}

template <class _Tp>
  requires(std::is_unbounded_array_v<_Tp>)
auto makeSharedForOverwrite(std::size_t __len) -> shared_ptr<_Tp> {
  std::remove_extent_t<_Tp> *__p = new std::remove_extent_t<_Tp>[__len];
  try {
    return SharedPtr<_Tp>(__p);
  } catch (...) {
    delete[] __p;
    throw;
  }
}

template <class _Tp, class _Up>
auto static_pointer_cast(shared_ptr<_Up> const &__ptr) -> shared_ptr<_Tp> {
  return shared_ptr<_Tp>(__ptr, static_cast<_Tp *>(__ptr.get()));
}

template <class _Tp, class _Up>
auto const_pointer_cast(shared_ptr<_Up> const &__ptr) -> shared_ptr<_Tp> {
  return shared_ptr<_Tp>(__ptr, const_cast<_Tp *>(__ptr.get()));
}

template <class _Tp, class _Up>
auto dynamic_pointer_cast(shared_ptr<_Up> const &__ptr) -> shared_ptr<_Tp> {
  _Tp *__p = dynamic_cast<_Tp *>(__ptr.get());
  return __p != nullptr ? shared_ptr<_Tp>(__ptr, __p) : nullptr;
}

template <class _Tp, class _Up>
auto reinterpret_pointer_cast(shared_ptr<_Up> const &__ptr) -> shared_ptr<_Tp> {
  return shared_ptr<_Tp>(__ptr, reinterpret_cast<_Tp *>(__ptr.get()));
}

} // namespace MySTL

#endif
