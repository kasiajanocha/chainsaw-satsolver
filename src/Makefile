DEPS = unit_propagation.h

DPLL: DPLL.cpp unit_propagation.cpp
	$(CXX) $(DEPS) $^ -o ${PROJ_DIR}/bin/$@ $(CXXFLAGS) -Wall -Wconversion -Wextra -Wpedantic -Wshadow

build:
	$(MAKE) CXXFLAGS+=-O3

debug: DPLL

.PHONY: build debug
