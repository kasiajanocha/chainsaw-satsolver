LIBS += -lpthread

_INCLUDE_DIRS += $(shell pwd)/include
INCLUDE_DIRS = $(foreach dir,$(_INCLUDE_DIRS), -I$(dir))

export INCLUDE_DIRS

GTEST = gtest/gtest_main.o gtest/libgtest.a 

test.x:
	$(MAKE) -C gtest
	$(MAKE) -C src
	$(CXX) $(CXXFLAGS) $(GTEST) src/*.o -o bin/$@ $(LIBS)

clean:
	$(MAKE) -C src clean
	$(MAKE) -C gtest clean
	$(RM) -f bin/*

.PHONY: clean
