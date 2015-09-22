#include <gtest/gtest.h>

TEST(Example2, NotEqual) {
    ASSERT_NEQ(1, 2) << "1 not equal 2";
}

TEST(Example2, ReallyNotEqual) {
    ASSERT_NEQ(1, 2) << "1 not equal 2";
}