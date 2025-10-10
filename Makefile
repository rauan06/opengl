CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -lGL -lGLU -lglut -lGLEW

# Source directory
SRCDIR = src

# Targets
red_triangle: $(SRCDIR)/red_triangle.cpp
	$(CXX) $(CXXFLAGS) -o red_triangle $(SRCDIR)/red_triangle.cpp $(LDFLAGS)

blue_square: $(SRCDIR)/blue_square.cpp
	$(CXX) $(CXXFLAGS) -o blue_square $(SRCDIR)/blue_square.cpp $(LDFLAGS)

task2_part1: $(SRCDIR)/task2_part1.cpp
	$(CXX) $(CXXFLAGS) -o task2_part1 $(SRCDIR)/task2_part1.cpp $(LDFLAGS)

task2_part2: $(SRCDIR)/task2_part2.cpp
	$(CXX) $(CXXFLAGS) -o task2_part2 $(SRCDIR)/task2_part2.cpp $(LDFLAGS)

all: red_triangle blue_square task2_part1 task2_part2

clean:
	rm -f red_triangle blue_square task2_part1 task2_part2

.PHONY: all clean
