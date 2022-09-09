#include "gtest/gtest.h"

#include "UnorderedMap.h"

struct Base {
    Base() = default;

    Base(const Base& s) = default;

    Base(Base&& s) : a(s.a) {
//        cout << "I`m moved\n";
    }

    Base& operator=(Base&& s) {
//        cout << "I`m operator= move\n";
    }

    Base(int a_) : a(a_) {}

    int a;
};

TEST(UnorderedMap, simpleTest) {
    UnorderedMap<int, Base> m;
    for (size_t i = 0; i < 10; ++i) {
        m.insert({i, Base(i)});
    }
    ASSERT_EQ(10, m.size());
    UnorderedMap<int, Base> m2;
    for (size_t i = 9; i < 19; ++i) {
        m2.insert({i, Base(i)});
    }
    ASSERT_EQ(10, m2.size());
    auto end = m2.begin();
    for (int i = 0; i < 9; i++) { ++end; };
    m.insert(m2.begin(), end);
    ASSERT_EQ(19, m.size());
    m.erase(m.begin());
    ASSERT_EQ(18, m.size());
    m.emplace(2, Base(2));
}