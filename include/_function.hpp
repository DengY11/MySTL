#ifndef _FUNCTION_HPP
#define _FUNCTION_HPP

#include <cassert>
#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>
namespace MySTL {

template <class _FnSig> struct function {
  // 使用了不合符Ret(args...)的模式进入此特化导致报错
  static_assert(!std::is_same_v<_FnSig, _FnSig>,
                "not a valid function signature");
};

template <class _Ret, class... _Args> struct function<_Ret(_Args...)> {
private:
  struct _FuncBase {
    virtual auto _M_call(_Args... __args) -> _Ret = 0;
    virtual auto _M_clone() const -> std::unique_ptr<_FuncBase> = 0;
    virtual auto _M_type() const -> std::type_info const & = 0;
    virtual ~_FuncBase() = default;
  };

  template <class _Fn> struct _FuncImpl : _FuncBase {
    _Fn _M_f;

    template <class... _CArgs>
    explicit _FuncImpl(std::in_place_t, _CArgs &&...__args)
        : _M_f(std::forward<_CArgs>(__args)...) {}

    auto _M_call(_Args... __args) -> _Ret override {
      return std::invoke(_M_f, std::forward<_Args>(__args)...);
    }

    auto _M_clone() const -> std::unique_ptr<_FuncBase> override {
      return std::make_unique<_FuncImpl>(std::in_place, _M_f);
    }

    auto _M_type() const -> std::type_info const & override {
      return typeid(_Fn);
    }
  };

  std::unique_ptr<_FuncBase> _M_base;

public:
  function() = default;
  function(std::nullptr_t) noexcept : function() {}

  template <class _Fn>
    requires(std::invocable<_Fn, _Args...>)        // 可调用
            && (std::is_copy_constructible_v<_Fn>) // 可拷贝
            && (!std::is_same_v<
                   std::decay_t<_Fn>,
                   function<_Ret(_Args...)>>) // 确保_Fn不是Function本身
  function(_Fn &&__f) // 没有explicit，允许lambda表达式隐式转换为Function
      : _M_base(std::make_unique<_FuncImpl<std::decay_t<_Fn>>>(
            std::in_place, std::forward<_Fn>(__f))) {}

  function(function &&) = default;

  auto operator=(function &&) -> function & = default;

  function(function const &__that)
      : _M_base(__that._M_base ? __that._M_base->_M_clone() : nullptr) {}

  auto operator=(function const &__that) -> function & {
    if (this != &__that) {
      _M_base = __that._M_base ? __that._M_base->_M_clone() : nullptr;
    }
    return *this;
  }

  explicit operator bool() const noexcept { return _M_base != nullptr; }

  bool operator==(std::nullptr_t) const noexcept { return _M_base == nullptr; }

  bool operator!=(std::nullptr_t) const noexcept { return _M_base != nullptr; }

  auto operator()(_Args... __args) const -> _Ret {
    if (!_M_base) [[unlikely]] {
      throw std::bad_function_call();
    }
    return _M_base->_M_call(std::forward<_Args>(__args)...);
  }

  auto target_type() const noexcept -> std::type_info const & {
    return _M_base ? _M_base->_M_type() : typeid(void);
  }
  template <class _Fn> auto target() const noexcept -> _Fn * {
    return _M_base && typeid(_Fn) == _M_base->_M_type()
               ? std::addressof(
                     static_cast<_FuncImpl<_Fn> *>(_M_base.get())->_M_f)
               : nullptr;
  }

  void swap(function &__that) noexcept { _M_base.swap(__that._M_base); }
};

} // namespace MySTL

#endif
