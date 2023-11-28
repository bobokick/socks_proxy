#ifndef GTEST_SAMPLE_H_
#define GTEST_SAMPLE_H_

#include <gtest/gtest.h>
#include <vector>

using namespace std;

// Test Fixtures creation
struct VectorTest: ::testing::Test
{
protected:
    vector<int> v1;
    vector<int> v2;
    vector<int> v3;
    void SetUp() override
    {
        v1 = {2,5,8};
        v2 = {-9,5,8,13};
        v3 = {6,-9,5};
    }
    void TearDown() override {};
};

// using a fixture, use TEST_F() instead of TEST() as it allows you to access objects and subroutines in the test fixture
TEST_F(VectorTest, HandleSizeEqual)
{
    EXPECT_NE(v1.size(), vector<int>({}).size());
    EXPECT_EQ(v1.size(), v3.size());
    EXPECT_NE(v2.size(), vector<int>({3,5}).size());
    EXPECT_EQ(vector<int>({2}).size(), vector<int>({1}).size());
}

TEST_F(VectorTest, HandleValEqual)
{
    for (int i = 0; i < static_cast<int>(v1.size()); ++i)
    {
        EXPECT_EQ(v1[i], vector<int>({2,5,8})[i]);
        // EXPECT_EQ(v1[i], v3[i]);
    }
}

// TEST(VectorTest, HandleSizeEqual)
// {
//     EXPECT_EQ(vector<int>({2,5,8}).size(), vector<int>({}).size());
//     EXPECT_EQ(vector<int>({8,6}).size(), vector<int>({9,55}).size());
//     EXPECT_EQ(vector<int>({-9,5,8,13}).size(), vector<int>({3,5}).size());
//     EXPECT_EQ(vector<int>({2}).size(), vector<int>({1}).size());
// }

void test1(vector<int> x, vector<int> y)
{
    ASSERT_EQ(x.size(), y.size()) << "Vectors x and y are of unequal length";
    for (int i = 0; i < static_cast<int>(x.size()); ++i)
        EXPECT_EQ(x[i], y[i]) << "Vectors x and y differ at index " << i;
}

// Most users should not need to write their own main function and instead link with gtest_main (as opposed to with gtest), which defines a suitable entry point.
// int main()
// {
//     // test1({2,5,8},{8,5,3});
//     // test1({2,5,8},{2,5,8});
//     return 0;
// }

#endif // !GTEST_SAMPLE_H_
