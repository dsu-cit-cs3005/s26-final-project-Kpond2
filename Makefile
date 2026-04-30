CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic

all: RobotWarz

RobotWarz: main.o Arena.o RobotBase.o
	$(CXX) $(CXXFLAGS) main.o Arena.o RobotBase.o -ldl -o RobotWarz

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

Arena.o: Arena.cpp Arena.h
	$(CXX) $(CXXFLAGS) -c Arena.cpp

RobotBase.o: RobotBase.cpp RobotBase.h
	$(CXX) $(CXXFLAGS) -c RobotBase.cpp

clean:
	rm -f *.o *.so RobotWarz
