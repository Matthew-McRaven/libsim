#pragma once

template <typename T, int C>
struct loop
{
  template<int _C=C, typename std::enable_if<_C >= 1, void>::type* = nullptr>
  void fn(T& ref) {
    //Recurse
    loop<T, _C-1>().fn(ref);
    std::cout << C;
    ref.element(emscripten::index<C>());
  }

  template <int _C = C, typename std::enable_if<_C == 0, void>::type* = nullptr>
  void fn(T& ref) {
    std::cout << C;
    ref.element(emscripten::index<0>());
  }
};