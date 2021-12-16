BOOSTPATH = "D:\Program Files\boost_1_77_0"

CXX = g++
CXXFLAGS = -std=c++17 -I $(BOOSTPATH)

std:
	$(CXX) $(CXXFLAGS) -o std.exe std.cpp

mpl:
	$(CXX) $(CXXFLAGS) -o mpl.exe mpl.cpp

hana:
	$(CXX) $(CXXFLAGS) -o hana.exe hana.cpp

all: std mpl hana

clean:
	del /Q *.exe
