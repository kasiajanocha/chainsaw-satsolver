#include "sat_include_all.h"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
    cdimacsIO io(std::cin, std::cout, std::cerr);

    solver<cdimacsIO,
           clause,
           valuation<TriBool>,
           randomizedLiteralFeeder<computation_context <clause, valuation<TriBool>>>
          > solver(io);
    solver.fillFormula();
    solver.run();
}
