#include <gtest/gtest.h>

TEST(Example1, Equal) {
    ASSERT_EQ(1, 2) << "1 not equal 2";
}

TEST(Example1, ReallyEqual) {
    ASSERT_EQ(1, 1) << "1 equal 1";
}



