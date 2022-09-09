#pragma once
#include <type_traits>
#include <memory>
#include <type_traits>

template<typename T, typename Allocator = std::allocator<T>>
class List {
public:
    explicit List(const Allocator& alloc = Allocator());

    explicit List(size_t count, const T& value = T(), const Allocator& alloc = Allocator());

    List(const List& other);
    List& operator=(const List& list);
    ~List();

    Allocator& get_allocator() const {
        return alloc_;
    }

    void pop_back();
    void pop_front();
    void push_front(const T& value);
    void push_back(const T& value);

    size_t size() const {
        return sz_;
    }

    void clear() {
        delete_nodes();
    }
private:
    void delete_nodes();

    struct BaseNode {
        BaseNode* next_ = nullptr;
        BaseNode* prev_ = nullptr;
    };

    struct Node : BaseNode {
        T value_;
    };

    BaseNode fake_node_;
    size_t sz_ = 0;

    using RebindAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using AllocTraits = std::allocator_traits<RebindAlloc>;
    RebindAlloc alloc_;

    template<bool IsConst>
    class common_iterator {
    private:
        friend class List;
        BaseNode* ptr_;
        using ConditionalPtr = std::conditional_t<IsConst, const T, T>;
    public:
        explicit common_iterator(BaseNode* node) : ptr_(node) {}
        ConditionalPtr& operator*() const {
            return static_cast<Node*>(ptr_)->value_; // TODO UB!
        }
        common_iterator& operator--() {
          ptr_ = ptr_->prev_;
            return *this;
        }
        common_iterator& operator++() {
          ptr_ = ptr_->next_;
            return *this;
        }
        common_iterator& operator++(int) {
            ++*this;
            return *this;
        }
        common_iterator& operator--(int) {
            --*this;
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

    template<typename Iterator>
    class common_reverse_iterator {
    private:
        Iterator iter_;
        using ConditionalReversePtr = std::conditional_t<std::is_same_v<const_iterator, Iterator>, const T, T>;
    public:
        explicit common_reverse_iterator(const Iterator& iter) : iter_(iter) {}

        ConditionalReversePtr& operator*() const {
            return *iter_;
        }
        common_reverse_iterator& operator++() {
            --iter_;
            return *this;
        }
        common_reverse_iterator& operator++(int) {
            --iter_;
            return *this;
        }
        common_reverse_iterator& operator--() {
            ++iter_;
            return *this;
        }
        common_reverse_iterator& operator--(int) {
            ++iter_;
            return *this;
        }
        bool operator==(const common_reverse_iterator<Iterator>& it) {
            return this->iter_ == it.iter_;
        }
        Iterator base() const {
            return iter_;
        }
    };

    using reverse_iterator = common_reverse_iterator<iterator>;
    using const_reverse_iterator = common_reverse_iterator<const_iterator>;

public:
    iterator begin() {
        return iterator(fake_node_.next_);
    }
    iterator end() {
        return iterator(&fake_node_);
    }
    const_iterator begin() const {
        return const_iterator(fake_node_.next_);
    }
    const_iterator end() const {
        return const_iterator(fake_node_.next_->prev_); // TODO fake_node_ is const, but ctr const_iterator get BaseNode*
    }
    const_iterator cbegin() {
        return const_iterator(fake_node_.next_);
    }
    const_iterator cend() {
        return const_iterator(&fake_node_);
    }
    reverse_iterator rbegin() {
        return reverse_iterator(iterator(fake_node_.prev_));
    }
    reverse_iterator rend() {
        return reverse_iterator(iterator(&fake_node_));
    }
    const_reverse_iterator rcbegin() {
        return const_reverse_iterator(const_iterator(fake_node_.prev_));
    }
    const_reverse_iterator rcend() {
        return const_reverse_iterator(const_iterator(&fake_node_));
    }

    template<bool isConstIter>
    void insert(const common_iterator<isConstIter>& it, const T& value);
    template<bool isConstIter>
    void erase(const common_iterator<isConstIter>& it);
};

#include "../../src/task_3/List.tpp"