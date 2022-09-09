#include <iostream>
#include <utility>

template<
    typename Key,
    typename Value,
    typename Hash = std::hash<Key>,
    typename Equal = std::equal_to<Key>,
    typename Allocator = std::allocator<std::pair<const Key, Value>>
>
class UnorderedMap {
 public:
  using NodeType = std::pair<const Key, Value>;

  UnorderedMap(size_t n = 1000,
               const Hash& hasher = Hash(),
               const Equal& key_equal = Equal(),
               const Allocator& alloc = Allocator());

  UnorderedMap(UnorderedMap&& other) noexcept;

  UnorderedMap(const UnorderedMap& other);

  UnorderedMap& operator=(const UnorderedMap& other);

  UnorderedMap& operator=(UnorderedMap&& other);

  ~UnorderedMap() = default;

 private:
  class List {
   public:
    List(const Allocator& alloc);

    List(const List& other);

    List(List&& other) noexcept;

    auto& operator=(List&& other);

    struct BaseNode {
      BaseNode* next_ = nullptr;
      BaseNode* prev_ = nullptr;
    };
    struct Node : BaseNode {
      NodeType node_;
      size_t cached_hash_;
    };

    BaseNode fake_node_;
    size_t sz_ = 0;

    using RebindAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using AllocTraits = std::allocator_traits<RebindAlloc>;
    RebindAlloc alloc_;

   public:

    const auto& get_allocator() const;

    auto* begin() const;

    auto* end() const;

    ~List();

   private:
    void delete_nodes();

   public:

    template<typename T>
    auto* push_back(T&& value, const size_t cached_hash);

    template<typename T>
    void insert(Node* ptr, T&& value, const size_t cached_hash);

    void erase(Node* ptr);

    size_t size() const;
  };

 private:
  constexpr static const double MAX_LOAD_FACTOR = 0.75;
  double load_factor_ = 0.;
  size_t size_of_buckets_ = 1000;
  List nodes_;
  Hash hasher_;
  Equal key_equal_;

  using ListBaseNode = typename List::BaseNode;
  using ListNode = typename List::Node;

  std::vector<ListNode*> buckets_;

  void delete_table() {
    nodes_.delete_nodes();
    buckets_.clear();
  }

  template<bool IsConst>
  class common_iterator {
   private:
    friend class UnorderedMap;

    ListBaseNode* ptr_;
    using ConditionalPtr = std::conditional_t<IsConst, const NodeType, NodeType>;
   public:
    explicit common_iterator(ListBaseNode* node_ptr) : ptr_(node_ptr) {}

    ConditionalPtr& operator*() const {
      return static_cast<ListNode*>(ptr_)->node_;
    }

    ConditionalPtr* operator->() const {
      return &static_cast<ListNode*>(ptr_)->node_;
    }

    common_iterator& operator++() {
      ptr_ = ptr_->next_;
      return *this;
    }

    common_iterator& operator++(int) {
      ++*this;
      return *this;
    }

    bool operator==(const common_iterator& it) {
      return ptr_->next_ == it.ptr_->next_ and
          ptr_->prev_ == it.ptr_->prev_;
    }

    bool operator!=(const common_iterator& it) {
      return !(*this == it);
    }
  };

  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;

 public:
  auto insert(const NodeType& nodeType) -> std::pair<iterator, bool>;

  std::pair<iterator, bool> insert(NodeType&& nodeType) {
    return inside_insert(std::move(nodeType));
  }

 private:
  template<typename T>
  auto inside_insert(T&& value) -> std::pair<iterator, bool>;

 public:

  void insert(iterator first, const iterator second);

  template<typename... Args>
  auto emplace(Args&& ... args) -> std::pair<iterator, bool>;

  void erase(const iterator erased);

  void erase(iterator first, const iterator second);

  iterator find(const Key& key) const {
    size_t find_hash = hasher_(key) % size_of_buckets_;
    ListNode* find_node = buckets_[find_hash];
    while (find_node and find_node->cached_hash_ == find_hash) {
      if (key_equal_(key, find_node->node_.first)) {
        return iterator(static_cast<ListBaseNode*>(find_node));
      }
      find_node = static_cast<ListNode*>(static_cast<ListBaseNode*>(find_node)->next_); // TODO
    }
    return iterator(nodes_.end());
  }

  Value& at(const Key& key) { // throw std::out_of_range
    size_t find_hash = hasher_(key) % size_of_buckets_;
    ListNode* find_node = buckets_[find_hash];
    while (find_node and find_node->cached_hash_ == find_hash) {
      if (key_equal_(key, find_node->node_.first)) {
        return find_node->node_.second;
      }
      find_node = static_cast<ListNode*>(static_cast<ListBaseNode*>(find_node)->next_); // TODO
    }
    throw std::out_of_range("Haven`t node_\n");
  }

  Value& operator[](const Key& key) {
    size_t find_hash = hasher_(key) % size_of_buckets_;
    ListNode* find_node = buckets_[find_hash];
    while (find_node and find_node->cached_hash_ == find_hash) {
      if (key_equal_(key, find_node->node_.first)) {
        return find_node->node_.second;
      }
      find_node = static_cast<ListNode*>(static_cast<ListBaseNode*>(find_node)->next_); // TODO
    }
    if constexpr(std::is_default_constructible_v<Value>) {
      return (this->insert({key, Value()})).first->second;
    }
  }

  void reserve(const size_t size) {
    size_of_buckets_ = size;
  }

  double load_factor() const { return load_factor_; }

  double max_load_factor() const { return MAX_LOAD_FACTOR; }

  double max_load_factor(const double max_load_factor) {
    load_factor_ = max_load_factor;
  }

  void rehash() {
    size_of_buckets_ *= 2;
    buckets_.clear();
    decltype(buckets_) new_buckets(size_of_buckets_, nullptr);
    buckets_ = new_buckets;
    // TODO
  }

  size_t size() const { return nodes_.size(); }

  iterator begin() noexcept {
    return iterator(nodes_.begin());
  }

  iterator end() noexcept {
    return iterator(nodes_.end());
  }

  const_iterator begin() const noexcept {
    return const_iterator(nodes_.begin());
  }

  const_iterator end() const noexcept {
    return const_iterator(nodes_.end());
  }

  const_iterator cbegin() noexcept {
    return const_iterator(nodes_.begin());
  }

  const_iterator cend() noexcept {
    return const_iterator(nodes_.end());
  }
};

#include "../../src/task_4/UnorderedMap.tpp"