DPLL: DPLL.cpp
	$(CXX) $^ -o ${PROJ_DIR}/bin/$@ $(CXXFLAGS) -Wall -Wconversion -Wextra -Wpedantic -Wshadow

build:
	$(MAKE) CXXFLAGS+=-O3

debug: DPLL

.PHONY: build debug
