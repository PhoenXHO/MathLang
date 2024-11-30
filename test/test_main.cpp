#include <gtest/gtest.h>

//TODO: Add test cases here
//! The code below is temporary and will be removed to be replaced by actual test cases

// Sample test case
// TEST(TestSuiteName, TestName): TestSuiteName is the name of the test suite and TestName is the name of the test case.
TEST(MathiumTest, BasicAssertions) {
    EXPECT_EQ(1 + 1, 2);
    EXPECT_TRUE(true);
}

int main(int argc, char **argv) {
	// Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
	// Run all tests
    return RUN_ALL_TESTS();
}
