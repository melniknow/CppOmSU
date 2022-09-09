
#include <iostream>

template<typename T, typename Alloc = std::allocator<T>>
class vector {
 public:
  vector() = default;

  vector(const size_t n);

  ~vector();

  vector(const vector& other);

  vector(vector&& other) noexcept;

  vector& operator=(const vector& other);

  vector& operator=(vector&& other);

  void resize(const size_t new_size);

  void push_back(const T& value);

  template<typename... Args>
  void emplace_back(Args&& ... args);

  void push_back(T&& value);

  void pop_back();

  size_t size() const;

  size_t capacity() const;

  T& operator[](const size_t pos) const;

 private:
  T* arr_ = nullptr;

  size_t cap_ = 0;
  size_t sz_ = 0;

  using AllocTraits = std::allocator_traits<Alloc>;
  Alloc alloc;

  void deallocate();
};

#include "../../src/task_1/vector.tpp"
