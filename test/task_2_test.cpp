#include "gtest/gtest.h"

#include "queue.h"

TEST(queue, simpleTest_my_queue) {
    queue<int> q(3);
    q.push(1);
    q.push(2);
    q.push(3);

    q.pop();
    q.pop();
    q.pop();

    q.push(1);
    q.push(2);
    q.push(3);

    int i = 0;
    for(auto item : q) {
        ASSERT_EQ(i++, item);
    }

}

TEST(queue, copy_ctr) {

}

