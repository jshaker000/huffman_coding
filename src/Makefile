#To add a main file, put its object file in the _OBJ and _MAINS list,
#copy an existing main rule and replace its trail with the appropriate object file

#To add a header template / implementation pair, add the header to _DEPS and the
#implementation to _OBJ
INSTALLDIR=/usr/local/bin

SRCDIR=.
IDIR=$(SRCDIR)/../include
ODIR=$(SRCDIR)/../obj

CXX=g++
CXXFLAGS=-std=c++11 -O2 -I$(IDIR)

_DEPS = bit_writer.h huffman_tree.h ancillary_functions.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ= huffman_encode.o huffman_decode.o huffman_codes.o huffman_tree.o bit_writer.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_MAINS = huffman_encode.o huffman_decode.o huffman_codes.o
MAINS = $(patsubst %,$(ODIR)/%,$(_MAINS))

all: huffman_encode huffman_decode huffman_codes

install: all
	cp $(SRCDIR)/huffman_encode $(INSTALLDIR)/huffman_encode
	cp $(SRCDIR)/huffman_decode $(INSTALLDIR)/huffman_decode
	cp $(SRCDIR)/huffman_codes  $(INSTALLDIR)/huffman_codes

uninstall:
	rm -f $(INSTALLDIR)/huffman_encode $(INSTALLDIR)/huffman_decode $(INSTALLDIR)/huffman_codes

remove:
	rm -f $(SRCDIR)/huffman_encode $(SRCDIR)/huffman_decode $(SRCDIR)/huffman_codes

$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

huffman_encode: $(filter-out $(MAINS), $(OBJ)) $(ODIR)/huffman_encode.o
	$(CXX) -o $@ $^ $(CXXFLAGS) 

huffman_decode: $(filter-out $(MAINS), $(OBJ)) $(ODIR)/huffman_decode.o
	$(CXX) -o $@ $^ $(CXXFLAGS) 

huffman_codes: $(filter-out $(MAINS), $(OBJ)) $(ODIR)/huffman_codes.o
	$(CXX) -o $@ $^ $(CXXFLAGS) 

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
