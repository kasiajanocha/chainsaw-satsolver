LIBS += -lpthread

_INCLUDE_DIRS += $(shell pwd)/include
INCLUDE_DIRS = $(foreach dir,$(_INCLUDE_DIRS), -I$(dir))
INCLUDE_DIRS += -I${PROJ_DIR}/include

$(shell mkdir -p bin)

export INCLUDE_DIRS

GTEST = gtest/gtest_main.o gtest/libgtest.a 

test.x: 
	$(MAKE) -C gtest
	@./gen_benchmark_tests.sh src/benchmarks.cc
	$(MAKE) -C src
	$(CXX) $(CXXFLAGS) $(GTEST) src/*.o -o bin/$@ $(LIBS)

clean:
	$(MAKE) -C src clean
	$(MAKE) -C gtest clean
	$(RM) -f bin/*

.PHONY: clean
