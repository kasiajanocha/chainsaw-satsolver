CXXFLAGS = -std=c++11 -Og -g
PROJ_DIR := $(shell pwd)

export CXXFLAGS
export PROJ_DIR

build: clean
	$(MAKE) CXXFLAGS+=-O3 -C src/

all: build runtest

clean:
	$(RM)  ${PROJ_DIR}/bin/*

cleanall: clean
	$(MAKE) -C test clean

debug: clean
	$(MAKE) -C src/

test:
	$(MAKE) -C test

runtest: test
	@./test/bin/test.x

.PHONY: build clean debug test runtest all
