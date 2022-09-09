#pragma once

template<
    typename T,
    typename Less = std::less<T>,
    typename Alloc = std::allocator<T>
>
class BinaryTree {
 public:
  BinaryTree(const Less& less = Less());

  ~BinaryTree();

  void insert(const T& value);
  void insert(T&& value);

  template<typename U>
  void emplace_insert(U&& value);

  void erase(const T& value);

  T& find(const T& value);

  template<typename CallBack>
  void forEach(CallBack call_back);

 private:
  struct BaseNode;

  template<typename U>
  BaseNode* findAndAllocateInsertPos(U&& value);

  template<typename U>
  void insertFirstItem(U&& value);

  void repaintParents(BaseNode* ptr);

  void leftRepetition(BaseNode* ptr);

  void rightRepetition(BaseNode* ptr);

  template<typename CallBack>
  void print(BaseNode* ptr, CallBack call_back);

  void deleter(BaseNode* ptr);

  enum Color {
    BLACK = 0,
    RED
  };

  struct BaseNode {
    BaseNode* parent = nullptr;
    BaseNode* left = nullptr;
    BaseNode* right = nullptr;
    Color color;
  };

  struct Node : BaseNode {
    T value;
  };

  using RebindAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
  using AllocTraits = std::allocator_traits<RebindAlloc>;

  BaseNode* root = nullptr;
  BaseNode fictitious_black_node_;
  Less less_;
  RebindAlloc alloc_;
};

#include "../../src/task_5/BinaryTree.tpp"