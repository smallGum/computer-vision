#include <gtest/gtest.h>
#include "hw1.h"

TEST(GcdTest, IntTest)
{
    CImg<unsigned char> img("1.bmp");
    EXPECT_EQ(true, task1(img));
    EXPECT_EQ(true, task2(img));
    EXPECT_EQ(true, task3(img));
    EXPECT_EQ(true, task4(img));
}
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
