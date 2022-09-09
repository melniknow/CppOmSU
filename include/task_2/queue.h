#include <stdexcept>

template<typename T, typename Alloc = std::allocator<T>>
class queue {
 public:
  queue(const size_t cap);

  void push(const T& value);

  void pop();

  T& front();

  size_t size() const;

  bool is_empty();

  void clear();

  ~queue();

 private:
  template<bool IsConst>
  class common_iterator {
   private:
    T* ptr_ = nullptr;
    T* start_ = nullptr;
    size_t cap_of_buffer_ = 0;
    using ConditionalPtr = std::conditional_t<IsConst, const T, T>;
   public:
    explicit common_iterator(T* ptr, const size_t cap) : ptr_(ptr), start_(ptr), cap_of_buffer_(cap) {}

    ConditionalPtr& operator*() const {
      return *ptr_;
    }

    ConditionalPtr* operator->() const {
      return ptr_;
    }

    common_iterator& operator++() {
      ptr_ = (ptr_ == start_ + (cap_of_buffer_ - 2)) ? start_ : ptr_ + 1;
      return *this;
    }

    common_iterator& operator++(int) {
      ++*this;
      return *this;
    }

    bool operator==(const common_iterator& it) {
      return ptr_ = it.ptr_;
    }

    bool operator!=(const common_iterator& it) {
      return !(*this == it);
    }
  };

  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;

 public:

  iterator begin();

  iterator end();

  const_iterator begin() const;

  const_iterator end() const;

 private:
  T* buffer_ = nullptr;
  size_t cap_ = 0;
  size_t sz_ = 0;
  size_t head_ = 0;
  size_t tail_ = 0;

  using AllocTraits = std::allocator_traits<Alloc>;
  Alloc alloc_;
};

#include "../../src/task_2/queue.tpp"