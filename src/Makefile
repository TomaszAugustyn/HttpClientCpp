CXX = g++
CXXFLAGS = -g -Wall

IDIR = ../include
ODIR = ../build
LIB_DIR = ../lib
INCS = -I$(IDIR) -L$(LIB_DIR)
LIBS = -lcurl -ljsoncpp -lpthread

SOURCES = main.cpp Addition.cpp Multiply.cpp HttpClient.cpp UserInputValidator.cpp TemperatureSensor.cpp Device.cpp

_HEADERS = Addition.hpp Multiply.hpp HttpClient.hpp UserInputValidator.hpp IValidator.hpp TemperatureSensor.hpp Device.hpp
HEADERS = $(patsubst %,$(IDIR)/%,$(_HEADERS))

_OBJS = $(SOURCES:.cpp=.o)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

EXECUTABLE = $(ODIR)/runHttpClient

.PHONY: all
all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(ODIR)/%.o : %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS)

run:
	$(EXECUTABLE)

.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJS)

cleanAll:
	rm -f $(ODIR)/*
