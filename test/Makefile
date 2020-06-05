CXX = g++
CXXFLAGS = -g -Wall -std=c++17

IDIR = ../include
ODIR = ../build
SRC_DIR = ../src
LIB_DIR = ../lib

INCS = -I./ -I$(SRC_DIR) -I$(IDIR) -L$(LIB_DIR)
LIBS = -lgtest -lgtest_main -lcurl -ljsoncpp -lpthread
VPATH = $(SRC_DIR)

_OBJS = UserInputValidator.o UserInputValidatorTest.o Device.o TemperatureSensor.o HttpClient.o HttpClientTest.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

EXECUTABLE = $(ODIR)/testAll

$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@  Main_TestAll.cpp $(OBJS) $(INCS) $(LIBS)

$(ODIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS) $(LIBS)

runTests:
	../build/testAll --gtest_output="xml:./testAll.xml"

.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJS) testAll.xml

cleanAll:
	rm -f $(ODIR)/* testAll.xml

