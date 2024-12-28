#ifndef DEFAULT_DELETER_HPP
#define DEFAULT_DELETER_HPP
#include <type_traits>
#include <utility>

namespace MySTL {

template<class _Tp> struct DefaultDeleter {

  void operator()(_Tp *p) const { delete p; }
};

template<class _Tp> struct DefaultDeleter<_Tp[]> {
  void operator()(_Tp *p) const { delete[] p; }
};

} // namespace MySTL

#endif
