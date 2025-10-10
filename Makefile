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

task3_3d_cube: $(SRCDIR)/task3_3d_cube.cpp
	$(CXX) $(CXXFLAGS) -o task3_3d_cube $(SRCDIR)/task3_3d_cube.cpp $(LDFLAGS)

task3_part1: $(SRCDIR)/task3_part1.cpp
	$(CXX) $(CXXFLAGS) -o task3_part1 $(SRCDIR)/task3_part1.cpp $(LDFLAGS)

all: task2_part1 task2_part2 task3_3d_cube task3_part1

clean:
	rm -f red_triangle blue_square task2_part1 task2_part2 task3_3d_cube assignment2_interaction assignment3_3d_cube

.PHONY: all clean
