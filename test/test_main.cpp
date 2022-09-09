#include "gtest/gtest.h"

#include "task_1_test.cpp"
#include "task_2_test.cpp"
#include "task_3_test.cpp"
#include "task_4_test.cpp"
#include "task_5_test.cpp"
#include "task_6_test.cpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

