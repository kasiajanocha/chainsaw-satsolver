CXXFLAGS = -Og -g

DPLL: DPLL.cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) -std=c++11 -Wall -Wconversion -Wextra -Wpedantic -Wshadow

build: clean
	$(MAKE) CXXFLAGS=-O3 DPLL

clean:
	$(RM) DPLL

.PHONY: build clean
