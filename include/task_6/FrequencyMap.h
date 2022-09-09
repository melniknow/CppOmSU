#include "BinaryTree.h"

#include <string>

template<typename T = std::string, typename Less = std::less<T>, typename Alloc = std::allocator<T>>
class FrequencyMap {
 public:
  FrequencyMap() = default;
  ~FrequencyMap() = default;

  void insert(const T& value);
  void insert(T&& value);

  template<typename U>
  size_t find(U&& value);

  size_t calculateAll();

  void printAll();
 private:

  template<typename U>
  void inner_insert(U&& value);

  BinaryTree<T, Less, Alloc> tree_;
};