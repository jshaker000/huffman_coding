#To add a main file, put its object file in the _OBJ and _MAINS list,
#copy an existing main rule and replace its trail with the appropriate object file

#To add a header template / implementation pair, add the header to _DEPS and the
#implementation to _OBJ
INSTALLDIR=/usr/local/bin

SRCDIR=$(PWD)
IDIR  =$(PWD)/../include
ODIR  =$(PWD)/../obj
CXX=g++
CXXFLAGS=-std=c++11 -O2 -I$(IDIR)

MAIN1=huffman_encode
MAIN2=huffman_decode
MAIN3=huffman_codes

_DEPS = bit_writer.h huffman_tree.h ancillary_functions.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ= $(MAIN1).o $(MAIN2).o $(MAIN3).o huffman_tree.o bit_writer.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_MAINS = $(MAIN1).o $(MAIN2).o $(MAIN3).o
MAINS = $(patsubst %,$(ODIR)/%,$(_MAINS))

all: huffman_encode huffman_decode huffman_codes

install: all
	cp $(SRCDIR)/$(MAIN1) $(INSTALLDIR)/$(MAIN1)
	cp $(SRCDIR)/$(MAIN2) $(INSTALLDIR)/$(MAIN2)
	cp $(SRCDIR)/$(MAIN3)  $(INSTALLDIR)/$(MAIN3)

uninstall:
	rm -f $(INSTALLDIR)/$(MAIN1) $(INSTALLDIR)/$(MAIN2) $(INSTALLDIR)/$(MAIN3)

remove:
	rm -f $(SRCDIR)/$(MAIN1) $(SRCDIR)/$(MAIN2) $(SRCDIR)/$(MAIN3)

$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(MAIN1): $(filter-out $(MAINS), $(OBJ)) $(ODIR)/$(MAIN1).o
	$(CXX) -o $@ $^ $(CXXFLAGS) 

$(MAIN2): $(filter-out $(MAINS), $(OBJ)) $(ODIR)/$(MAIN2).o
	$(CXX) -o $@ $^ $(CXXFLAGS) 

$(MAIN3): $(filter-out $(MAINS), $(OBJ)) $(ODIR)/$(MAIN3).o
	$(CXX) -o $@ $^ $(CXXFLAGS) 

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
