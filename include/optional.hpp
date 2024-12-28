#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include <exception>
#include <initializer_list>
#include <type_traits>
namespace MySTL {

struct bad_optional_access : std::exception {
  bad_optional_access() = default;
  virtual ~bad_optional_access() = default;

  const char *what() const noexcept override { return "bad optional access"; }
};

struct nullopt_t {
  explicit nullopt_t() = default;
};

constexpr nullopt_t nullopt;

struct in_place_t {
  explicit in_place_t() = default;
};

constexpr in_place_t in_place;

template<class T> struct optional {
private:
  bool m_has_value;
  union {
    T m_value;
  };

public:
  optional(T &&value) noexcept: m_has_value(true), m_value(std::move(value)) {}

  optional(const T &value) noexcept
      : m_has_value(true), m_value(std::move(value)) {}

  optional() noexcept: m_has_value(false) {}

  optional(nullopt_t) noexcept: m_has_value(false) {}

  template<class... Ts>
  explicit optional(in_place_t, Ts &&...value_args)
      : m_has_value(true), m_value(std::forward<Ts>(value_args)...) {}

  template<class U, class... Ts>
  explicit optional(in_place_t, std::initializer_list<U> ilist,
                    Ts &&...value_args)
      : m_has_value(true), m_value(ilist, std::forward<Ts>(value_args)...) {}

  optional(optional const &that) : m_has_value(that.m_has_value) {
    if (m_has_value) {
      new(&m_value) T(that.m_value);
    }
  }

  optional(optional &&that) noexcept: m_has_value(that.m_has_value) {
    if (m_has_value) {
      new(&m_value) T(std::move(that.m_value));
    }
  }

  optional &operator=(nullopt_t) noexcept {
    if (m_has_value) {
      m_value.~T();
      m_has_value = false;
    }
    return *this;
  }

  optional &operator=(T &&value) noexcept {
    if (m_has_value) {
      m_value.~T();
      m_has_value = false;
    }
    new(&m_value) T(std::move(value));
    m_has_value = true;
    return *this;
  }

  optional &operator=(T const &value) noexcept {
    if (m_has_value) {
      m_value.~T();
      m_has_value = false;
    }
    new(&m_value) T(value);
    m_has_value = true;
    return *this;
  }

  optional &operator=(optional const &that) {
    if (this == &that) {
      return *this;
    }

    if (m_has_value) {
      m_value.~T();
      m_has_value = false;
    }

    if (that.m_has_value) {
      new(&m_value) T(that.m_value);
    }
    m_has_value = that.m_has_value;
    return *this;
  }

  optional &operator=(optional &&that) {
    if (this == &that) {
      return *this;
    }

    if (m_has_value) {
      m_value.~T();
      m_has_value = false;
    }

    if (that.m_has_value) {
      new(&m_value) T(that.m_value);
      that.m_value.~T();
    }
    m_has_value = that.m_has_value;
    that.m_has_value = false;
    return *this;
  }

  template<class... Ts> void emplace(Ts &&...value_args) {
    if (m_has_value) {
      m_value.~T();
      m_has_value = false;
    }

    new(&m_value) T(std::forward<Ts>(value_args)...);
    m_has_value = true;
  }

  template<class U, class... Ts>
  void emplace(std::initializer_list<U> ilist, Ts &&...value_args) {
    if (m_has_value) {
      m_value.~T();
      m_has_value = false;
    }
    new(&m_value) T(ilist, std::forward<Ts>(value_args)...);
    m_has_value = true;
  }

  void reset() noexcept {
    if (m_has_value) {
      m_value.~T();
      m_has_value = false;
    }
  }

  ~optional() noexcept {
    if (m_has_value) {
      m_value.~T();
    }
  }

  auto has_value() const noexcept -> bool { return m_has_value; }

  explicit operator bool() const noexcept { return m_has_value; }

  auto operator==(nullopt_t) const noexcept -> bool { return !m_has_value; }

  friend auto operator==(nullopt_t, optional const &self) noexcept -> bool {
    return !self.m_has_value;
  }

  auto operator!=(nullopt_t) const noexcept { return m_has_value; }

  friend auto operator!=(nullopt_t, optional const &self) noexcept -> bool {
    return self.m_has_value;
  }

  auto value() const & -> T const & {
    if (!m_has_value) {
      throw bad_optional_access();
    }
    return m_value;
  }

  auto value() & -> T & {
    if (!m_has_value) {
      throw bad_optional_access();
    }
    return m_value;
  }

  auto value() const && -> T const && {
    if (!m_has_value) {
      throw bad_optional_access();
    }
    return std::move(m_value);
  }

  auto value() && -> T && {
    if (!m_has_value) {
      throw bad_optional_access();
    }
    return std::move(m_value);
  }

  auto operator*() const & noexcept -> T const & { return m_value; }

  auto operator*() & noexcept -> T & { return m_value; }

  auto operator*() const && noexcept -> T const && {
    return std::move(m_value);
  }

  auto operator*() && noexcept -> T && { return std::move(m_value); }

  auto operator->() const noexcept -> T const * { return &m_value; }

  auto operator->() noexcept -> T * { return &m_value; }

  auto value_or(T default_value) const & -> T {
    if (!m_has_value) {
      return default_value;
    }
    return m_value;
  }

  auto value_or(T default_value) && noexcept -> T {
    if (!m_has_value) {
      return default_value;
    }
    return std::move(m_value);
  }

  auto operator==(optional<T> const &that) const noexcept -> bool {
    if (m_has_value != that.m_has_value) {
      return false;
    }
    if (m_has_value) {
      return m_value == that.m_value;
    }
    return true;
  }

  auto operator!=(optional<T> const &that) const noexcept -> bool {
    if (m_has_value != that.m_has_value) {
      return false;
    }
    if (m_has_value) {
      return m_value != that.m_value;
    }
    return false;
  }

  auto operator<(optional const &that) const noexcept -> bool {
    if (!m_has_value || !that.m_has_value) {
      return false;
    }
    return m_value < that.m_value;
  }

  auto operator>(optional const &that) const noexcept -> bool {
    if (!m_has_value || !that.m_has_value) {
      return false;
    }
    return m_value > that.m_value;
  }

  auto operator>=(optional const &that) const noexcept -> bool {
    if (!m_has_value || !that.m_has_value) {
      return false;
    }
    return m_value >= that.m_value;
  }

  auto operator<=(optional const &that) const noexcept -> bool {
    if (!m_has_value || !that.m_has_value) {
      return false;
    }
    return m_value <= that.m_value;
  }

  template<class F>
  auto and_then(F &&f) const & -> std::remove_cvref_t<decltype(f(m_value))> {
    if (m_has_value) {
      return std::forward<F>(f)(m_value);
    } else {
      return std::remove_cvref_t<decltype(f(m_value))>{};
    }
  }

  template<class F>
  auto and_then(F &&f) & -> std::remove_cvref_t<decltype(f(m_value))> {
    if (m_has_value) {
      return std::forward<F>(f)(m_value);
    } else {
      return std::remove_cvref_t<decltype(f(m_value))>{};
    }
  }

  template<class F>
  auto and_then(F &&f) const && -> std::remove_cvref_t<decltype(f(m_value))> {
    if (m_has_value) {
      return std::forward<F>(f)(std::move(m_value));
    } else {
      return std::remove_cvref_t<decltype(f(std::move(m_value)))>{};
    }
  }

  template<class F>
  auto and_then(F &&f) && -> std::remove_cvref_t<decltype(f(m_value))> {
    if (m_has_value) {
      return std::forward<F>(f)(std::move(m_value));
    } else {
      return std::remove_cvref_t<decltype(f(std::move(m_value)))>{};
    }
  }

  template<class F>
  auto transform(
      F &&f) const & -> optional<std::remove_cvref_t<decltype(f(m_value))>> {
    if (m_has_value) {
      return std::forward<F>(f)(m_value);
    } else {
      return nullopt;
    }
  }

  template<class F>
  auto
  transform(F &&f) & -> optional<std::remove_cvref_t<decltype(f(m_value))>> {
    if (m_has_value) {
      return std::forward<F>(f)(m_value);
    } else {
      return nullopt;
    }
  }

  template<class F>
  auto transform(F &&f) const
  && -> optional<std::remove_cvref_t<decltype(f(std::move(m_value)))>> {
    if (m_has_value) {
      return std::forward<F>(f)(std::move(m_value));
    } else {
      return nullopt;
    }
  }

  template<class F>
  auto transform(F &&f)
  && -> optional<std::remove_cvref_t<decltype(f(std::move(m_value)))>> {
    if (m_has_value) {
      return std::forward<F>(f)(std::move(m_value));
    } else {
      return nullopt;
    }
  }

  template<class F>
  requires(std::is_move_constructible_v<T>)
  auto or_else(F &&f) && -> optional {
    if (m_has_value) {
      return std::move(*this);
    } else {
      return std::forward<F>(f)();
    }
  }

  template<class F>
  requires(std::is_move_constructible_v<T>)
  auto or_else(F &&f) const & -> optional {
    if (m_has_value) {
      return std::move(*this);
    } else {
      return std::forward<F>(f)();
    }
  }

  void swap(optional &that) noexcept {
    if (m_has_value && that.m_has_value) {
      using std::swap;
      swap(m_value, that.m_value);
    } else if (!m_has_value && !that.m_has_value) {
      //
    } else if (m_has_value) {
      that.emplace(std::move(m_value));
    } else {
      emplace(std::move(that.m_value));
      that.reset();
    }
  }
};

template<class T> auto make_optional(T value) {
  return optional<T>(std::move(value));
}

#if __cpp_deduction_guides
template <class T> optional(T) -> optional<T>;
#endif

} // namespace MySTL

#endif
