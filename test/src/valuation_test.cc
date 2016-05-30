#include "gtest/gtest.h"

#include <sstream>

#include "sat_include_all.h"

TEST(ValuationTest, test_1)
{
    valuation<TriBool> val;
    int32_t size = 10;
    val.resize(10);

    for (int32_t idx = 2; idx <= size; idx += 2)
        val[idx] = TRUE;

    std::stringstream sstr;
    sstr << val;
    EXPECT_EQ(sstr.str(), " -1 2 -3 4 -5 6 -7 8 -9 10");
}
