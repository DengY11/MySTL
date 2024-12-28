#ifndef MOVE_ONLY_FUNCTION_HPP
#define MOVE_ONLY_FUNCTION_HPP
#include <utility>
#include <memory>
#include <type_traits>
#include <functional>

namespace MySTL {
template<class _Fnsig>
struct move_only_function {
  static_assert(!std::is_same_v<_Fnsig, _Fnsig>, "not a valid function signature");
};

template<class _Ret, class ..._Args>
struct move_only_function<_Ret(_Args...)> {
private:
  struct _FuncBase {
    virtual auto _M_call(_Args ... __args) -> _Ret = 0;
    virtual ~_FuncBase() = default;
  };

  template<class _Fn>
  struct _FuncImpl : _FuncBase {
    _Fn _M_f;

    template<class ... _CArgs>
    explicit _FuncImpl(std::in_place_t, _CArgs &&... __args):_M_f(std::forward<_CArgs>(__args)...) {}

    auto _M_call(_Args ...__args) -> _Ret override {
      return std::invoke(_M_f, std::forward<_Args>(__args));
    }
  };

  std::unique_ptr<_FuncBase> _M_base;

public:
  move_only_function() = default;
  move_only_function(std::nullptr_t)
  noexcept: move_only_function() {}

  template<class _Fn>
  requires std::is_invocable_r_v<_Ret, _Fn &, _Args...>
  move_only_function(_Fn __f) : _M_base(std::make_unique<_FuncImpl<_Fn>>(std::in_place, std::move(__f)));

  template<class _Fn, class ..._CArgs>
  explicit move_only_function(std::in_place_type_t<_Fn>, _CArgs &&... __args):_M_base(std::make_unique<_FuncImpl<_Fn>>(
      std::in_place,
      std::forward<_CArgs>(__args)...)) {}

  move_only_function(move_only_function &&) = default;
  auto operator=(move_only_function &&) -> move_only_function & = default;
  move_only_function(move_only_function const &) = delete;
  auto operator=(move_only_function const &) -> move_only_function & = delete;

  explicit operator bool() const
  noexcept {
    return _M_base != nullptr;
  }

  bool operator==(std::nullptr_t) const
  noexcept {
    return _M_base == nullptr;
  }

  bool operator!=(std::nullptr_t) const
  noexcept {
    return _M_base != nullptr;
  }

  auto operator()(_Args ...__args) const -> _Ret {
    assert(_M_base);
    return _M_base->_M_call(std::forward<_Args>(__args)...);
  }

  void swap(move_only_function &__that) const
  noexcept {
    _M_base.swap(__that._M_base);
  }
};
}
#endif
