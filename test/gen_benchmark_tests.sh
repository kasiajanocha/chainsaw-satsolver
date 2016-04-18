#!/bin/bash

TEST_PATH=`pwd`
BENCHMARKS=$(ls benchmarks | sed 's:\(\<.*\>\):"'"$TEST_PATH"'\/benchmarks\/\1":g' | tr "\n" "," | sed 's/,$//')

cat << EOF > $1

#include "gtest/gtest.h"
#include <iostream>
#include <string>

using namespace std;

struct BenchmarkTest : public ::testing::TestWithParam<const char*>
{};

TEST_P(BenchmarkTest, test_0)
{
    cerr << GetParam() << endl;
}

INSTANTIATE_TEST_CASE_P(T1, BenchmarkTest, ::testing::Values($BENCHMARKS));
EOF
