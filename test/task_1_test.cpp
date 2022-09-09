#include "gtest/gtest.h"

#include "vector.h"
#include <vector>

struct S {
    S(size_t a) : a(a) {}
    size_t a;
    S(const S& s) : a(s.a) {}
    S(S&& other) : a(other.a) {}
};

TEST(vector, simpleTest_my_vec) {
    vector<S> v;
    const int SIZE = 1000;
    for(size_t i = 0; i < SIZE; ++i) {
        v.push_back(S(i));
    }
    for (size_t i = 0; i < SIZE; ++i) {
        ASSERT_EQ(i, v[i].a);
    }
    ASSERT_EQ(SIZE, v.size());
}

TEST(vector, copy_ctr) {
    vector<S> v;
    const int SIZE = 1000;
    for(size_t i = 0; i < SIZE; ++i) {
        v.push_back(S(i));
    }
    vector<S> v2(v);
    for (size_t i = 0; i < v2.size(); ++i) {
        ASSERT_EQ(i, v[i].a);
    }

    v = v2;
    v2 = std::move(v);
}
