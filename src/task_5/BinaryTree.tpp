
template<typename T, typename Less, typename Alloc>
BinaryTree<T, Less, Alloc>::BinaryTree(const Less& less) : less_(less) {}

template<typename T, typename Less, typename Alloc>
BinaryTree<T, Less, Alloc>::~BinaryTree() {
  if (root != nullptr) {
    deleter(root);
  }
}

template<typename T, typename Less, typename Alloc>
void BinaryTree<T, Less, Alloc>::deleter(BinaryTree::BaseNode* ptr) {
  if (ptr == &fictitious_black_node_) {
    return;
  }
  deleter(ptr->left);
  BaseNode* tmp = ptr->right;
  AllocTraits::destroy(alloc_, static_cast<Node*>(ptr));
  AllocTraits::deallocate(alloc_, static_cast<Node*>(ptr), 1);
  deleter(tmp);
}

template<typename T, typename Less, typename Alloc>
void BinaryTree<T, Less, Alloc>::insert(const T& value) {
  emplace_insert(value);
}

template<typename T, typename Less, typename Alloc>
void BinaryTree<T, Less, Alloc>::insert(T&& value) {
  emplace_insert(std::move(value));
}

template<typename T, typename Less, typename Alloc>
template<typename U>
void BinaryTree<T, Less, Alloc>::emplace_insert(U&& value) {
  if (root == nullptr) {
    insertFirstItem(std::forward<U>(value));
    return;
  }

  BaseNode* insert_pos = findAndAllocateInsertPos(std::forward<U>(value));

  if (insert_pos->parent->color == Color::RED) {
    if (insert_pos->parent->parent->right != &fictitious_black_node_ &&
        insert_pos->parent->parent->right->color == Color::RED) { // uncle is RED
      repaintParents(insert_pos->parent);
      BaseNode* grandpa = insert_pos->parent->parent;
      while (grandpa->parent != nullptr && grandpa->parent->color == Color::RED) {
        repaintParents(grandpa->parent); // again repaint
        grandpa = grandpa->parent->parent;
      }
      if (grandpa == root) {
        root->color = Color::BLACK;
      }
    } else { // uncle is BLACK or nullptr
      if (insert_pos->parent->parent->right == &fictitious_black_node_) {
        insert_pos->color = Color::BLACK;
        return;
      } else if (insert_pos == insert_pos->parent->left) { // insert_pos is left child
        leftRepetition(insert_pos->parent->parent);
      } else { // insert_pos is right child
        rightRepetition(insert_pos->parent->parent);
        leftRepetition(insert_pos->parent->parent);
      }
    }
  }
}

template<typename T, typename Less, typename Alloc>
void BinaryTree<T, Less, Alloc>::leftRepetition(BinaryTree::BaseNode* ptr) {
  ptr->color = Color::RED;
  if (ptr == ptr->parent->left) {
    ptr->parent->left = ptr->left;
  } else {
    ptr->parent->right = ptr->left;
  }
  ptr->left->parent = ptr->parent;
  ptr->parent = ptr->left;
  BaseNode* tmp = ptr->left->right;
  tmp->parent = ptr;
  ptr->left->color = Color::BLACK;
  ptr->left->right = ptr;
  ptr->left = tmp;
}

template<typename T, typename Less, typename Alloc>
void BinaryTree<T, Less, Alloc>::rightRepetition(BinaryTree::BaseNode* ptr) {
  BaseNode* insert_left_child = ptr->left->left->left;
  BaseNode* insert_right_child = ptr->left->left->right;
  BaseNode* insert_right_child_y = ptr->left->right;

  ptr->left->left->parent = ptr;
  BaseNode* tmp = ptr->left;
  ptr->left = ptr->left->left;
  tmp->parent = ptr->left;
  ptr->left->left = tmp;
  tmp->right = insert_right_child;
  tmp->left = insert_left_child;
  ptr->left->right = insert_right_child_y;
}

template<typename T, typename Less, typename Alloc>
void BinaryTree<T, Less, Alloc>::repaintParents(BinaryTree::BaseNode* ptr) {
  ptr->color = Color::BLACK;
  ptr->parent->color = Color::RED;
  if (ptr->parent->right != &fictitious_black_node_) {
    ptr->parent->right->color = Color::BLACK;
  }
}

template<typename T, typename Less, typename Alloc>
template<typename U>
typename BinaryTree<T, Less, Alloc>::BaseNode* BinaryTree<T, Less, Alloc>::findAndAllocateInsertPos(U&& value) {
  auto ptr = root;
  bool is_left_child = true;
  while (ptr->right != &fictitious_black_node_ || ptr->left != &fictitious_black_node_) {

    if (less_(std::forward<U>(value), static_cast<Node*>(ptr)->value)) {
      if (ptr->left != &fictitious_black_node_) {
        ptr = ptr->left;
      } else {
        is_left_child = true;
        break;
      }
    } else {
      if (ptr->right != &fictitious_black_node_) {
        ptr = ptr->right;
        is_left_child = false;
      } else {
        break;
      }
    }
  }
  Node* new_ptr = AllocTraits::allocate(alloc_, 1);
  AllocTraits::construct(alloc_, new_ptr, Node{ptr,
                                               &fictitious_black_node_,
                                               &fictitious_black_node_,
                                               Color::RED,
                                               std::forward<U>(value)});
  if (ptr == root) {
    if (less_(std::forward<U>(value), static_cast<Node*>(root)->value)) {
      root->left = new_ptr;
      return new_ptr;
    } else {
      root->right = new_ptr;
      return new_ptr;
    }
  }
  if (is_left_child) {
    ptr->left = new_ptr;
  } else {
    ptr->right = new_ptr;
  }
  return new_ptr;
}

template<typename T, typename Less, typename Alloc>
template<typename U>
void BinaryTree<T, Less, Alloc>::insertFirstItem(U&& value) {
  Node* ptr = AllocTraits::allocate(alloc_, 1);
  AllocTraits::construct(alloc_, ptr, Node{nullptr,
                                           &fictitious_black_node_,
                                           &fictitious_black_node_,
                                           Color::BLACK,
                                           std::forward<U>(value)});
  root = static_cast<BaseNode*>(ptr);
}

template<typename T, typename Less, typename Alloc>
void BinaryTree<T, Less, Alloc>::erase(const T& value) {
  // TODO
}
template<typename T, typename Less, typename Alloc>
template<typename CallBack>
void BinaryTree<T, Less, Alloc>::forEach(CallBack call_back) {
  print(root, call_back);
}

template<typename T, typename Less, typename Alloc>
template<typename CallBack>
void BinaryTree<T, Less, Alloc>::print(BinaryTree::BaseNode* ptr, CallBack call_back) {
  if (ptr == &fictitious_black_node_) {
    return;
  }
  print(ptr->left, call_back);
  call_back(static_cast<Node*>(ptr)->value);
  print(ptr->right, call_back);
}

template<typename T, typename Less, typename Alloc>
T& BinaryTree<T, Less, Alloc>::find(const T& value) {
  BaseNode* ptr = root;
  while (ptr->right != &fictitious_black_node_ || ptr->left != &fictitious_black_node_) {
    if (less_(value, static_cast<Node*>(ptr)->value) == less_(static_cast<Node*>(ptr)->value, value) == 0) {
      return static_cast<Node*>(ptr)->value;
    }
    less_(value, static_cast<Node*>(ptr)->value) ?
        ptr = ptr->left :
        ptr = ptr->right;
  }
  return static_cast<Node*>(root)->value;
}