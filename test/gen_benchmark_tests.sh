#!/bin/bash

TEST_PATH=`pwd`
BENCHMARKS=$(ls benchmarks | sed 's:[^ ][^ ]*:"'"$TEST_PATH"'/benchmarks/&":g' | tr "\n" "," | sed 's/,$//')
cat << EOF > $1

#include "gtest/gtest.h"

#define VERBOSE 1
#include "sat_include_all.h"

#include <fstream>
#include <iostream>
#include <string>

struct BenchmarkTest : public ::testing::TestWithParam<const char*>
{};

TEST_P(BenchmarkTest, test_0)
{
    std::fstream fstr;
    fstr.open(GetParam(), std::fstream::in);
    std::cerr << "Benchmark : " << GetParam() << std::endl;

    dimacsIO<std::fstream, std::ostream, std::ostream> io(fstr, std::cout, std::cerr);
    solver<dimacsIO<std::fstream, std::ostream, std::ostream>,
           clause,
           valuation<TriBool>,
           randomizedLiteralFeeder<computation_context <clause, valuation<TriBool>>>
          > solver(io);
    solver.fillFormula();
    solver.run();

    fstr.close();
}

INSTANTIATE_TEST_CASE_P(T1, BenchmarkTest, ::testing::Values($BENCHMARKS));
EOF
