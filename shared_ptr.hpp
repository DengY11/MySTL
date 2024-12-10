#ifndef SHARED_PTR_HPP
#define SHARED_PTR_HPP
#include "default_deleter.hpp"
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

  template <class _Yp,
            std::enable_if_t<std::is_convertible_v<_Yp *, _Tp *>, int> = 0>
  explicit shared_ptr(_Yp *__ptr)
      : _M_ptr(__ptr),
        _M_owner(new _SpCounterImpl<_Yp, DefaultDeleter<_Yp>>(__ptr)) {
    _S_setupEnableSharedFromThis(_M_ptr, _M_owner);
  }

  // TODO:
  template <class _Yp, class _Deleter,
            std::enable_if_t<std::is_convertible_v<_Yp *, _Tp *>, int> = 0>
  explicit shared_ptr<>
};

} // namespace MySTL

#endif
