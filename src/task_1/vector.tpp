

template<typename T, typename Alloc>
vector<T, Alloc>::vector(const size_t n) : cap_(n) {
  arr_ = AllocTraits::allocate(alloc, cap_);
}

template<typename T, typename Alloc>
vector<T, Alloc>::~vector() {
  deallocate();
}

template<typename T, typename Alloc>
vector<T, Alloc>::vector(const vector& other) : sz_(other.sz_), cap_(other.cap_),
                                               alloc(AllocTraits::select_on_container_copy_construction(other.alloc)) {
  arr_ = AllocTraits::allocate(alloc, cap_);
  for (size_t i = 0; i < sz_; ++i) {
    AllocTraits::construct(alloc, arr_ + i, other[i]);
  }
}

template<typename T, typename Alloc>
vector<T, Alloc>::vector(vector&& other) noexcept : sz_(other.sz_), cap_(other.cap_), arr_(other.cap_),
                                                    alloc(AllocTraits::select_on_container_copy_construction(other.alloc)) {
  other.sz_ = other.cap_ = 0;
  other.arr_ = nullptr;
}

template<typename T, typename Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(const vector& other) {
  deallocate();
  if (this != &other) {
    sz_ = other.sz_;
    cap_ = other.cap_;
    alloc = AllocTraits::select_on_container_copy_construction(other.alloc);
    arr_ = AllocTraits::allocate(alloc, cap_);
    for (size_t i = 0; i < sz_; ++i) {
      AllocTraits::construct(alloc, arr_ + i, other[i]);
    }
  }
  return *this;
}

template<typename T, typename Alloc>
vector<T, Alloc>& vector<T, Alloc>::operator=(vector&& other) {
  if (this != &other) {
    deallocate();
    sz_ = other.sz_;
    cap_ = other.cap_;
    arr_ = other.arr_;
    alloc = AllocTraits::select_on_container_copy_construction(other.alloc);
    other.sz_ = other.cap_ = 0;
    other.arr_ = nullptr;
  }
  return *this;
}

template<typename T, typename Alloc>
void vector<T, Alloc>::resize(const size_t new_size) {
  if (new_size < cap_) return;

  T* new_arr = AllocTraits::allocate(alloc, new_size);

  size_t i = 0;
  try {
    for (; i < sz_; ++i) {
      AllocTraits::construct(alloc, new_arr + i, std::move_if_noexcept(arr_[i]));
    }
  } catch (...) {
    for (size_t j = 0; j < i; ++j) {
      AllocTraits::destroy(alloc, new_arr + j);
    }
    AllocTraits::deallocate(alloc, new_arr, new_size);
    throw;
  }

  for (i = 0; i < sz_; ++i) {
    AllocTraits::destroy(alloc, arr_ + i);
  }
  if (new_size != 1) {
    AllocTraits::deallocate(alloc, arr_, cap_);
  }

  arr_ = new_arr;
  cap_ = new_size;
}

template<typename T, typename Alloc>
void vector<T, Alloc>::push_back(const T& value) {
  if (sz_ == cap_) {
    resize(cap_ == 0 ? 1 : cap_ * 2);
  }
  AllocTraits::construct(alloc, arr_[sz_], value);
  ++sz_;
}

template<typename T, typename Alloc>
template<typename... Args>
void vector<T, Alloc>::emplace_back(Args&& ... args)  {
  if (sz_ == cap_) {
    resize(cap_ == 0 ? 1 : cap_ * 2);
  }
  AllocTraits::construct(alloc, arr_ + sz_, std::forward<Args>(args)...);
  ++sz_;
}

template<typename T, typename Alloc>
void vector<T, Alloc>::push_back(T&& value) {
  if (sz_ == cap_) {
    resize(cap_ == 0 ? 1 : cap_ * 2);
  }
  AllocTraits::construct(alloc, arr_ + sz_, std::move(value));
  ++sz_;
}

template<typename T, typename Alloc>
void vector<T, Alloc>::pop_back() {
  AllocTraits::destroy(alloc, arr_[sz_]);
  --sz_;
}

template<typename T, typename Alloc>
size_t vector<T, Alloc>::size() const {
  return sz_;
}

template<typename T, typename Alloc>
size_t vector<T, Alloc>::capacity() const {
  return cap_;
}

template<typename T, typename Alloc>
T& vector<T, Alloc>::operator[](const size_t pos) const {
  return *(arr_ + pos);
}

template<typename T, typename Alloc>
void vector<T, Alloc>::deallocate() {
  for (size_t i = 0; i < sz_; ++i) {
    AllocTraits::destroy(alloc, arr_ + i);
  }
  AllocTraits::deallocate(alloc, arr_, cap_);
}