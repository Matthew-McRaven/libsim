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

/*
 * I don't want to add getters / setters to my instruction definition classes.
 * This would massively populate a POD struct. However, getters/setters
 * are required in order to get emscripten to access a field.
 * 
 * Below are methods that act as if they are getters for a pointer to an
 * instruction definition class. Embind will treat these as-if they were
 * class member functions, because they have the right signature.
 * 
 * Infer the return types, so that we can interop between RISC-V and Pep9-10.
 * 
*/
template<typename T>
auto bit_pattern(const std::shared_ptr<T>& ref) -> decltype(ref->bit_pattern){
  return ref->bit_pattern;
};

template<typename T>
auto iformat(const std::shared_ptr<T>& ref) -> decltype(ref->iformat){
  return ref->iformat;
};

template<typename T>
auto CSR_modified(const std::shared_ptr<T>& ref) -> decltype(ref->CSR_modified){
  return ref->CSR_modified;
};

template<typename T>
auto mnemonic(const std::shared_ptr<T>& ref) -> decltype(ref->mnemonic){
  return ref->mnemonic;
};

template<typename T>
auto is_unary(const std::shared_ptr<T>& ref) -> decltype(ref->is_unary){
  return ref->is_unary;
};

template<typename T>
auto comment(const std::shared_ptr<T>& ref) -> decltype(ref->comment){
  return ref->comment;
};
