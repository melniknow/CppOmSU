#include "gtest/gtest.h"

#include "BinaryTree.h"

struct Move {
  int a;
  Move(int a) : a(a) {};
  Move(const Move& value) {
    std::cout << "I`m coped\n";
  }
  Move(Move&& other) : a(other.a) {
//    std::cout << "I`m moved\n";
  }
};

struct MoveLess {
  bool operator()(const Move& lhs, const Move& rhs) {
    return lhs.a < rhs.a;
  }
};

TEST(BinaryTree, simpleTest) {
  BinaryTree<int> b;
  b.insert(1);
  b.insert(2);

  b.insert(4);
  b.insert(5);
  b.insert(-1);
  b.insert(-5);
  b.insert(-3);
  b.insert(7);
  b.insert(6);
  b.insert(10);
  b.insert(9);
  b.insert(15);
  b.insert(13);
  b.insert(3);
}

TEST(BinaryTree, moveTest) {
  BinaryTree<Move, MoveLess> b;
  b.insert(Move(1));
  b.insert(Move(2));
  b.insert(Move(-3));
  b.insert(Move(7));
  b.insert(Move(8));
  b.insert(Move(6));
  b.insert(Move(-2));
  b.insert(Move(14));
  b.insert(Move(13));
  b.insert(Move(12));
  b.insert(Move(9));
  b.insert(Move(8));
}