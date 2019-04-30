#To add a main file, put its object file in the _OBJ and _MAINS list,
#copy an existing main rule and replace its trail with the appropriate object file

#To add a header template / implementation pair, add the header to _DEPS and the
#implementation to _OBJ
INSTALLDIR=/usr/local/bin
SRCDIR=${CURDIR}
IDIR  =${CURDIR}/../include
ODIR  =${CURDIR}/../obj
CXX=g++
CXXFLAGS=-std=c++11 -O2 -I$(IDIR)

MAIN1=huffman_encode
MAIN2=huffman_decode
MAIN3=huffman_codes

TARGETS = huffman_encode huffman_decode huffman_codes

_DEPS = bit_writer.h huffman_tree.h ancillary_functions.h
DEPS  = $(patsubst %,$(IDIR)/%,$(_DEPS))

_MAINS = $(addsuffix .o, $(TARGETS) )
MAINS = $(patsubst %,$(ODIR)/%,$(_MAINS))

_OBJ= huffman_tree.o bit_writer.o $(_MAINS)
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY: all install uninstall remove clean

all: $(MAIN1) $(MAIN2) $(MAIN3)

install: all
	cp -f $(SRCDIR)/$(MAIN1) $(INSTALLDIR)/$(MAIN1)
	cp -f $(SRCDIR)/$(MAIN2) $(INSTALLDIR)/$(MAIN2)
	cp -f $(SRCDIR)/$(MAIN3) $(INSTALLDIR)/$(MAIN3)

uninstall:
	rm -f $(INSTALLDIR)/$(MAIN1) $(INSTALLDIR)/$(MAIN2) $(INSTALLDIR)/$(MAIN3)

remove:
	rm -f $(SRCDIR)/$(MAIN1) $(SRCDIR)/$(MAIN2) $(SRCDIR)/$(MAIN3)

clean:
	rm -f $(ODIR)/*.o

$(OBJ): $(ODIR)/%.o : $(SRCDIR)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGETS): % : $(filter-out $(MAINS), $(OBJ)) $(ODIR)/%.o
	$(CXX) -o $@ $^ $(CXXFLAGS) 