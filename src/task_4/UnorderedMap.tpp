

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>::UnorderedMap(
    size_t n,
    const Hash& hasher,
    const Equal& key_equal,
    const Allocator& alloc)
    : size_of_buckets_(n),
      buckets_(size_of_buckets_, nullptr),
      key_equal_(key_equal),
      hasher_(hasher),
      nodes_(alloc) {}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>::UnorderedMap(const UnorderedMap& other) :
    load_factor_(other.load_factor_), size_of_buckets_(other.size_of_buckets_), buckets_(size_of_buckets_),
    nodes_(std::allocator_traits<Allocator>::select_on_container_copy_construction(other.nodes_.get_allocator())) {
  for (const auto node : other) {
    this->insert(node);
  }
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>::UnorderedMap(UnorderedMap&& other) noexcept
    : nodes_(std::move(other.nodes_)), buckets_(std::move(other.buckets_)), load_factor_(other.load_factor_),
      size_of_buckets_(other.size_of_buckets_) {
  other.load_factor_ = other.size_of_buckets_ = 0;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>&
UnorderedMap<Key, Value, Hash, Equal, Allocator>::operator=(const UnorderedMap& other) {
  if (this == &other) return *this;

  delete_table();
  for (const auto item : other) {
    this->insert(item);
  }
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>&
UnorderedMap<Key, Value, Hash, Equal, Allocator>::operator=(UnorderedMap&& other) {
  nodes_ = std::move(other.nodes_);
  buckets_ = std::move(other.buckets_);
  size_of_buckets_ = other.other.size_of_buckets_;
  load_factor_ = other.load_factor_;
  other.load_factor_ = other.size_of_buckets_ = 0;
  return *this;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
template<typename T>
auto UnorderedMap<Key, Value, Hash, Equal, Allocator>::inside_insert(T&& value) -> std::pair<iterator, bool> {
  size_t hash_of_node_type = hasher_(value.first) % size_of_buckets_;
  // ++load_factor_ and then rehash...
  if (!buckets_[hash_of_node_type]) {
    buckets_[hash_of_node_type] = nodes_.push_back(std::forward<T>(value),
                                                   hash_of_node_type);
    return {iterator(static_cast<ListBaseNode*>
                     (buckets_[hash_of_node_type % size_of_buckets_])), true};
  } else {
    ListNode* ptr = buckets_[hash_of_node_type];
    while (ptr->cached_hash_ == hash_of_node_type) {
      if (key_equal_(value.first, ptr->node_.first)) {
        return {iterator(nodes_.end()), false};
      }
      ptr = static_cast<ListNode*>(static_cast<ListBaseNode*>(ptr)->next_); // TODO
    }
    nodes_.insert(ptr, std::forward<T>(value), hash_of_node_type);
    return {iterator(static_cast<ListBaseNode*>(ptr)), true};
  }
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
void UnorderedMap<Key, Value, Hash, Equal, Allocator>::insert(UnorderedMap::iterator first,
                                                              const UnorderedMap::iterator second) {
  for (; first != second; first++) {
    this->insert(*first);
  }
  this->insert(*second);
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
template<typename... Args>
auto UnorderedMap<Key, Value, Hash, Equal, Allocator>::emplace(Args&& ... args) -> std::pair<iterator, bool> {
  return this->insert(std::make_pair(std::forward<Args>(args)...));
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
void UnorderedMap<Key, Value, Hash, Equal, Allocator>::erase(const UnorderedMap::iterator erased) {
  nodes_.erase(static_cast<ListNode*>(erased.ptr_));
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
void UnorderedMap<Key, Value, Hash, Equal, Allocator>::erase(UnorderedMap::iterator first,
                                                             const UnorderedMap::iterator second) {
  for (; first != second; ++first) {
    nodes_.erase(static_cast<ListNode*>(first.ptr_));
  }
  nodes_.erase(static_cast<ListNode*>(second.ptr_));
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
auto UnorderedMap<Key, Value, Hash, Equal, Allocator>::insert(const UnorderedMap::NodeType& nodeType)
  -> std::pair<iterator, bool> {
  return inside_insert(nodeType);
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>::List::List(const Allocator& alloc) : alloc_(
    std::allocator_traits<Allocator>::select_on_container_copy_construction(alloc)) {}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>::List::List(const UnorderedMap::List& other) : List(other.alloc_) {
  for (BaseNode* node = other.fake_node_.next_; node != &fake_node_;
       node = node->next_) {
    this->push_back(static_cast<Node*>(node)->node_, static_cast<Node*>(node)->cached_hash_);
  }
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>::List::List(UnorderedMap::List&& other) noexcept :
    alloc_(std::move(other.alloc_)), // ???
    fake_node_(other.fake_node_),
    sz_(other.sz_) {
  fake_node_.prev_->next_ = &fake_node_;
  other.fake_node_.next_ = other.fake_node_.prev_ = nullptr;
  other.sz_ = 0;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
auto& UnorderedMap<Key, Value, Hash, Equal, Allocator>::List::operator=(UnorderedMap::List&& other) {
  delete_nodes();
  alloc_(std::move(other.alloc_));
  fake_node_(other.fake_node_);
  fake_node_.prev_->next_ = &fake_node_;
  sz_(other.sz_);
  other.fake_node_.next_ = other.fake_node_.prev_ = nullptr;
  other.sz_ = 0;
  return *this;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
const auto& UnorderedMap<Key, Value, Hash, Equal, Allocator>::List::get_allocator() const {
  return alloc_;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
auto* UnorderedMap<Key, Value, Hash, Equal, Allocator>::List::begin() const {
  return fake_node_.next_;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
auto* UnorderedMap<Key, Value, Hash, Equal, Allocator>::List::end() const {
  return fake_node_.next_->prev_;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
UnorderedMap<Key, Value, Hash, Equal, Allocator>::List::~List() {
  delete_nodes();
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
void UnorderedMap<Key, Value, Hash, Equal, Allocator>::List::delete_nodes() {
  BaseNode* base_node = fake_node_.next_;
  while (base_node and base_node != &fake_node_) {
    BaseNode* tmp_node = base_node->next_;
    AllocTraits::destroy(alloc_, static_cast<Node*>(base_node));
    AllocTraits::deallocate(alloc_, static_cast<Node*>(base_node), 1);
    base_node = tmp_node;
  }
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
template<typename T>
auto* UnorderedMap<Key, Value, Hash, Equal, Allocator>::List::push_back(T&& value, const size_t cached_hash) {
  Node* new_node = AllocTraits::allocate(alloc_, 1);
  if (!fake_node_.prev_) {
    AllocTraits::construct(alloc_, new_node, Node{&fake_node_, &fake_node_, std::forward<T>(value), cached_hash});
    fake_node_.prev_ = new_node;
    fake_node_.next_ = new_node;
  } else {
    AllocTraits::construct(alloc_, new_node, Node{&fake_node_, fake_node_.prev_, std::forward<T>(value), cached_hash});
    fake_node_.prev_->next_ = new_node;
    fake_node_.prev_ = new_node;
  }
  ++sz_;
  return static_cast<Node*>(fake_node_.prev_);
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
template<typename T>
void UnorderedMap<Key, Value, Hash, Equal, Allocator>::List::insert(UnorderedMap::List::Node* ptr,
                                                                    T&& value,
                                                                    const size_t cached_hash) {
  Node* new_node = AllocTraits::allocate(alloc_, 1);
  AllocTraits::construct(alloc_, new_node, Node{ptr, ptr->prev_, std::forward<T>(value), cached_hash});
  ptr->prev_->next_ = new_node;
  ptr->prev_ = new_node;
  ++sz_;
}

template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
void UnorderedMap<Key, Value, Hash, Equal, Allocator>::List::erase(UnorderedMap::List::Node* ptr) {
  ptr->prev_->next_ = ptr->next_;
  ptr->next_->prev_ = ptr->prev_;
  AllocTraits::destroy(alloc_, static_cast<Node*>(ptr));
  AllocTraits::deallocate(alloc_, static_cast<Node*>(ptr), 1);
  --sz_;
}
template<typename Key, typename Value, typename Hash, typename Equal, typename Allocator>
size_t UnorderedMap<Key, Value, Hash, Equal, Allocator>::List::size() const {
  return sz_;
}
