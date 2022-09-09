

template<typename T, typename Allocator>
List<T, Allocator>::List(const Allocator& alloc) : alloc_(alloc) {}

template<typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
  Node* new_node = AllocTraits::allocate(alloc_, 1);
  if (!fake_node_.next_) {
    AllocTraits::construct(alloc_, new_node, Node{&fake_node_, &fake_node_, value});
    fake_node_.next_ = new_node;
    fake_node_.prev_ = new_node;
  } else {
    AllocTraits::construct(alloc_, new_node, Node{fake_node_.next_, &fake_node_, value});
    fake_node_.next_->prev_ = new_node;
    fake_node_.next_ = new_node;
  }
  ++sz_;
}

template<typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
  Node* new_node = AllocTraits::allocate(alloc_, 1);
  if (!fake_node_.prev_) {
    AllocTraits::construct(alloc_, new_node, Node{&fake_node_, &fake_node_, value});
    fake_node_.prev_ = new_node;
    fake_node_.next_ = new_node;
  } else {
    AllocTraits::construct(alloc_, new_node, Node{&fake_node_, fake_node_.prev_, value});
    fake_node_.prev_->next_ = new_node;
    fake_node_.prev_ = new_node;
  }
  ++sz_;
}

template<typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
  if (fake_node_.next_) {
    Node* remove_node = static_cast<Node*>(fake_node_.next_); // TODO BAD_CAST
    fake_node_.next_->next_->prev_ = &fake_node_;
    fake_node_.next_ = fake_node_.next_->next_;
    AllocTraits::destroy(alloc_, remove_node);
    AllocTraits::deallocate(alloc_, remove_node, 1);
    sz_--;
  }
}

template<typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
  if (fake_node_.prev_) {
    Node* remove_node = static_cast<Node*>(fake_node_.prev_); // TODO BAD_CAST
    fake_node_.prev_->prev_->next_ = &fake_node_;
    fake_node_.prev_ = fake_node_.prev_->prev_;
    AllocTraits::destroy(alloc_, remove_node);
    AllocTraits::deallocate(alloc_, remove_node, 1);
    sz_--;
  }
}

template<typename T, typename Allocator>
template<bool isConstIter>
void List<T, Allocator>::insert(const List::common_iterator<isConstIter>& it, const T& value) {
  Node* new_node = AllocTraits::allocate(alloc_, 1);
  AllocTraits::construct(alloc_, new_node, Node{it.ptr_, it.ptr_->prev_, value});
  it.ptr_->prev_->next_ = new_node;
  it.ptr_->prev_ = new_node;
  ++sz_;
}

template<typename T, typename Allocator>
template<bool isConstIter>
void List<T, Allocator>::erase(const List::common_iterator<isConstIter>& it) {
  it.ptr_->prev_->next_ = it.ptr_->next_;
  it.ptr_->next_->prev_ = it.ptr_->prev_;
  AllocTraits::destroy(alloc_, static_cast<Node*>(it.ptr_));
  AllocTraits::deallocate(alloc_, static_cast<Node*>(it.ptr_), 1);
}

template<typename T, typename Allocator>
void List<T, Allocator>::delete_nodes() {
  BaseNode* base_node = fake_node_.next_;
  while (base_node != &fake_node_ and base_node) {
    BaseNode* tmp_node = base_node->next_;
    AllocTraits::destroy(alloc_, static_cast<Node*>(base_node));
    AllocTraits::deallocate(alloc_, static_cast<Node*>(base_node), 1);
    base_node = tmp_node;
  }
  sz_ = 0;
}

template<typename T, typename Allocator>
List<T, Allocator>::~List() {
  delete_nodes();
}

template<typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& alloc)
    : alloc_(AllocTraits::select_on_container_copy_construction(alloc)) {
  for (size_t i = 0; i < count; ++i) {
    this->push_back(value);
  }
}

template<typename T, typename Allocator>
List<T, Allocator>::List(const List& other) { // TODO
  for (const auto iter : other) {
    this->push_back(iter);
  }
}

template<typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& list) = default;