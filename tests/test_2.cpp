#include <gtest/gtest.h>

TEST(Example2, NotEqual) {
    ASSERT_NE(1, 1) << "1 equal 1";
}

TEST(Example2, ReallyNotEqual) {
    ASSERT_NE(1, 2) << "1 not equal 2";
}