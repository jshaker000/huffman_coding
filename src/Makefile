# To add a main file, put it in "TARGETS"
# This assumes it will have a respective cpp file
# copy an existing main rule and replace its trail with the appropriate object file

# All headers must be in DEPS because they will require full compiling if they redefine function
# All headers are assumed to have a mathing cpp file

INSTALLDIR := /usr/local/bin
SRCDIR     := ${CURDIR}
IDIR       := ${CURDIR}/../include
ODIR       := ${CURDIR}/../obj
CXX        := g++
CXXFLAGS   := -std=c++17 -O3 -flto -Wall -Werror -Wextra -pedantic -I${IDIR}

TARGETS := huffman_encode huffman_decode huffman_codes

_DEPS   := bit_writer.h huffman_encode_tree.h huffman_decode_tree.h print_functions.h
DEPS    := $(patsubst %,${IDIR}/%,${_DEPS})

_MAINS  := $(addsuffix .o, ${TARGETS})
MAINS   := $(patsubst %,${ODIR}/%,${_MAINS})

_OBJ    := $(patsubst %.h,%.o,${_DEPS}) ${_MAINS}
OBJ     := $(patsubst %,${ODIR}/%,${_OBJ})

.PHONY: all install uninstall remove clean

all: ${TARGETS}

install: all
	for i in ${TARGETS}; do cp -f $$i ${INSTALLDIR}/$$i; done

uninstall:
	for i in ${TARGETS}; do rm -f ${INSTALLDIR}/$$i; done

remove:
	rm -f ${TARGETS}

clean: remove
	rm -f ${ODIR}/*.o

${OBJ}: ${ODIR}/%.o : ${SRCDIR}/%.cpp $(DEPS)
	${CXX} -c -o $@ $< ${CXXFLAGS}

${TARGETS}: % : $(filter-out ${MAINS}, ${OBJ}) ${ODIR}/%.o
	${CXX} -o $@ $^ ${CXXFLAGS}
