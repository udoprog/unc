VERSION=1.0

SOURCES+=src/unc.cpp

DATA_SOURCES+=gen/iso8859.cpp
DATA_SOURCES+=gen/unccase.cpp

OBJECTS=$(SOURCES:.cpp=.o)
DATA_OBJECTS=$(DATA_SOURCES:.cpp=.o)

PYTHON=/usr/bin/env python --
TOOLS=${PYTHON} tools.py
SPECS=$(wildcard specs/*.TXT)
NAMESPACE="unc"

PREFIX=/usr/local
INCLUDE=${PREFIX}/include/unc
LIBRARY=${PREFIX}/lib
PKGCONFIG=${PREFIX}/lib/pkgconfig

CXXFLAGS=-I./src -I./gen -Wall

M4FLAGS=-DPREFIX=${PREFIX} -DVERSION=${VERSION}
M4=m4

all: lib/libuncdata.a lib/libunc.a

lib:
	mkdir $@

lib/libunc.a: lib ${OBJECTS}
	ar rcs $@ ${OBJECTS}

lib/libuncdata.a: lib ${DATA_OBJECTS}
	ar rcs $@ ${DATA_OBJECTS}

gen:
	mkdir $@

gen/iso8859.cpp: gen ${SPECS}
	${TOOLS} specs $@ gen/iso8859.hpp iso8859.hpp ${NAMESPACE}

gen/unccase.cpp: gen specs/UnicodeData.txt
	${TOOLS} unicodedata specs/UnicodeData.txt $@ gen/unccase.hpp unccase.hpp ${NAMESPACE}

clean:
	${RM} ${OBJECTS}
	${RM} lib/libunc.a
	${RM} lib/libuncdata.a
	rm -rf lib
	rm -rf gen

install:
	mkdir -p ${INCLUDE}
	mkdir -p ${PKGCONFIG}
	cp gen/iso8859.hpp ${INCLUDE}/iso8859.hpp
	cp gen/unccase.hpp ${INCLUDE}/unccase.hpp
	cp src/unc.hpp ${INCLUDE}/unc.hpp
	cp lib/libunc.a ${LIBRARY}/libunc.a.${VERSION}
	cp lib/libuncdata.a ${LIBRARY}/libuncdata.a.${VERSION}
	ln -fs ${LIBRARY}/libunc.a.${VERSION} ${LIBRARY}/libunc.a
	ln -fs ${LIBRARY}/libuncdata.a.${VERSION} ${LIBRARY}/libuncdata.a
	${M4} ${M4FLAGS} < pkgconfig/unc.pc.m4 > ${PKGCONFIG}/unc.pc
	-ln -fs ${PKGCONFIG}/unc.pc /usr/lib/pkgconfig/unc.pc

tests: all
	sh tests.sh test ${CXX} -Wall -I./src -L./lib

unicodedata:
	${TOOLS} unicodedata specs/UnicodeData.txt
