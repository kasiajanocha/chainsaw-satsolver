
test: gtest_main.o libgtest.a 
	@echo "Google test libraries built"

libgtest.a: libgtest.o
	ar rcs $@ $^

gtest_main.o: gtest_main.cc
	g++ -Wall ${INCLUDE_DIRS} -std=c++11 -c $^ -o $@

libgtest.o: gtest-all.cc
	g++ -Wall -static ${INCLUDE_DIRS} -std=c++11 -c $^ -o $@
clean:
	$(RM) -f ../bin/* ./*.o ./*.a

.PHONY: clean
