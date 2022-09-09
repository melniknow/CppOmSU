#include "gtest/gtest.h"

#include "List.h"

TEST(List, simple_test) {
    List<int> my_list;
    my_list.push_back(1);
    my_list.push_back(2);
    auto it = my_list.begin();
    ASSERT_EQ(1, *it);
    ASSERT_EQ(2, *++it);
    my_list.insert(my_list.begin(), 7);

    ASSERT_EQ(7, *my_list.begin());
    ASSERT_EQ(3, my_list.size());
    my_list.insert(my_list.cbegin(), 9);
    ASSERT_EQ(9, *my_list.cbegin());


    my_list.pop_back();
    ASSERT_EQ(3, my_list.size());
    my_list.pop_back();
    my_list.pop_back();
    my_list.pop_back();
}


