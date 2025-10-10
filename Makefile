CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -lGL -lGLU -lglut -lGLEW

# Source directory
SRCDIR = src

# Targets
task2_part1: $(SRCDIR)/task2_part1.cpp
	$(CXX) $(CXXFLAGS) -o task2_part1 $(SRCDIR)/task2_part1.cpp $(LDFLAGS)

task2_part2: $(SRCDIR)/task2_part2.cpp
	$(CXX) $(CXXFLAGS) -o task2_part2 $(SRCDIR)/task2_part2.cpp $(LDFLAGS)

all: task2_part1 task2_part2 

clean:
	rm -f red_triangle blue_square task2_part1 task2_part2 assignment2_interaction assignment3_3d_cube

.PHONY: all clean
