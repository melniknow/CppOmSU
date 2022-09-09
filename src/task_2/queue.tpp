
template<typename T, typename Alloc>
queue<T, Alloc>::queue(const size_t cap) : cap_(cap + 1) {
  buffer_ = AllocTraits::allocate(alloc_, cap_);
}

template<typename T, typename Alloc>
void queue<T, Alloc>::push(const T& value) {
  if (sz_ == cap_ - 1) {
    throw std::out_of_range("Haven`t a space");
  }
  AllocTraits::construct(alloc_, buffer_ + tail_, value);
  tail_ = (tail_ + 1) % cap_;
  ++sz_;
}

template<typename T, typename Alloc>
void queue<T, Alloc>::pop() {
  if (sz_ == 0) {
    throw std::out_of_range("Haven`t a space");
  }
  AllocTraits::destroy(alloc_, buffer_ + head_);
  head_ = (head_ + 1) % cap_;
  --sz_;
}

template<typename T, typename Alloc>
T& queue<T, Alloc>::front() {
  return *(buffer_ + head_);
}

template<typename T, typename Alloc>
size_t queue<T, Alloc>::size() const {
  return sz_;
}

template<typename T, typename Alloc>
bool queue<T, Alloc>::is_empty() {
  return sz_ == 0;
}

template<typename T, typename Alloc>
void queue<T, Alloc>::clear() {
  for (int i = 0; i < sz_; ++i) {
    AllocTraits::destroy(alloc_, buffer_ + i);
  }
  AllocTraits::deallocate(alloc_, buffer_, cap_);
}
template<typename T, typename Alloc>
queue<T, Alloc>::~queue() {
  clear();
}

template<typename T, typename Alloc>
typename queue<T, Alloc>::const_iterator queue<T, Alloc>::end() const {
  return const_iterator(buffer_ + tail_, cap_);
}

template<typename T, typename Alloc>
typename queue<T, Alloc>::const_iterator queue<T, Alloc>::begin() const {
  return const_iterator(buffer_, cap_);
}

template<typename T, typename Alloc>
typename queue<T, Alloc>::iterator queue<T, Alloc>::begin() {
  return iterator(buffer_, cap_);
}

template<typename T, typename Alloc>
typename queue<T, Alloc>::iterator queue<T, Alloc>::end() {
  return iterator(buffer_ + tail_, cap_);
}
