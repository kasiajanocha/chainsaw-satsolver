CXX = g++
CXXFLAGS = -std=c++11 -g
PROJ_DIR := $(shell pwd)

$(shell mkdir -p ${PROJ_DIR}/bin)

export CXXFLAGS
export PROJ_DIR

build: clean
ifeq ($(build_type),satori)
	$(CXX) $(CXXFLAGS) -O3 *.cpp -o solution
else
	$(MAKE) CXXFLAGS+=-O3 -C src/
endif

all: build runtest

clean:
	$(RM) -r ${PROJ_DIR}/bin/* ${PROJ_DIR}/satori*

cleanall: clean
	$(MAKE) -C test clean

debug: clean
	$(MAKE) -C src/

test:
	$(MAKE) -C test

runtest: cleanall test
	@./test/bin/test.x

satori:
	@mkdir -p ${PROJ_DIR}/satori ;	\
	cp ${PROJ_DIR}/src/*.cpp ${PROJ_DIR}/satori ;	\
	cp ${PROJ_DIR}/include/*.h ${PROJ_DIR}/satori ;	\
	echo "build_type = satori"  > ${PROJ_DIR}/satori/Makefile;	\
	cat ${PROJ_DIR}/Makefile >> ${PROJ_DIR}/satori/Makefile;	\
	zip -j satori.zip ${PROJ_DIR}/satori/*

.PHONY: build clean debug test runtest all satori
