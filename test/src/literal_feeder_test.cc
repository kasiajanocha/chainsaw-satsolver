#include "gtest/gtest.h"

#include <vector>

#include "sat_include_all.h"


struct DummyContext
{
    typedef int16_t literal_type;

    literal_type numVars;
};

TEST(SimpleLiteralFeederTest, test_1)
{
    DummyContext ctx;
    ctx.numVars = 10;
    simpleLiteralFeeder<DummyContext> sFeeder(ctx);

    std::vector<typename DummyContext::literal_type> expectedOrdering{1,2,3,4,5,6,7,8,9,10,0};
    for(const auto& i : expectedOrdering)
    {
        EXPECT_EQ(i, sFeeder.getLiteral());
    }
}

TEST(SimpleLiteralFeederTest, test_2)
{
    DummyContext ctx;
    ctx.numVars = 1;
    simpleLiteralFeeder<DummyContext> sFeeder(ctx);
    
    EXPECT_EQ(1, sFeeder.getLiteral());
    EXPECT_EQ(0, sFeeder.getLiteral());
    EXPECT_EQ(0, sFeeder.getLiteral());
}
