CXX = g++
CXXFLAGS = -g -Wall

IDIR = ../include
ODIR = ../build
LIB_DIR = ../lib
INCS = -I$(IDIR) -L$(LIB_DIR)

SOURCES = main.cpp Addition.cpp Multiply.cpp

_HEADERS = Addition.hpp Multiply.hpp
HEADERS = $(patsubst %,$(IDIR)/%,$(_HEADERS))

_OBJS = $(SOURCES:.cpp=.o)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

EXECUTABLE = $(ODIR)/exampleapp

.PHONY: all
all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(ODIR)/%.o : %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS)

run:
	$(EXECUTABLE)

.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJS)

cleanAll:
	rm -f $(ODIR)/*
