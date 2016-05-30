DEPS = unit_propagation.h
SRC = main.cpp
CUSTOM_FLAGS =

ifdef BUILD_VERBOSE
CUSTOM_FLAGS += -DVERBOSE
$(info  setting CUSTOM_FLAGS)
else
$(info CUSTOM_FLAGS unset)
endif

all: main.cpp
	$(CXX) -I ${PROJ_DIR}/include ${SRC} ${CUSTOM_FLAGS} -o ${PROJ_DIR}/bin/csatsolv $(CXXFLAGS) -Wall -Wconversion -Wextra -Wpedantic -Wshadow 


DPLL: DPLL.cpp unit_propagation.cpp
	$(CXX) $(DEPS) $^ -o ${PROJ_DIR}/bin/$@ $(CXXFLAGS) -Wall -Wconversion -Wextra -Wpedantic -Wshadow

build:
	$(MAKE) CXXFLAGS+=-O3

debug: DPLL

.PHONY: build debug
