SRC = $(wildcard *.cc)
OBJS = $(SRC:.cc=.o)
QC_OBJ = ss_test.o

all: ${OBJS}

clean:
	$(RM) ${OBJS}

%.o: %.cc
	$(CXX) ${INCLUDE_DIRS} $(CXXFLAGS) -c -o $@ $<

.PHONY: clean all
