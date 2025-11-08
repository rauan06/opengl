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

assignment4_part1: $(SRCDIR)/assignment4_part1_bezier.cpp
	$(CXX) $(CXXFLAGS) -o assignment4_part1 $(SRCDIR)/assignment4_part1_bezier.cpp $(LDFLAGS)

assignment4_part2: $(SRCDIR)/assignment4_part2_picking.cpp
	$(CXX) $(CXXFLAGS) -o assignment4_part2 $(SRCDIR)/assignment4_part2_picking.cpp $(LDFLAGS)

assignment4_part3a: $(SRCDIR)/assignment4_part3a_texture_bezier.cpp
	$(CXX) $(CXXFLAGS) -o assignment4_part3a $(SRCDIR)/assignment4_part3a_texture_bezier.cpp $(LDFLAGS)

assignment4_part3b: $(SRCDIR)/assignment4_part3b_3d_texture.cpp
	$(CXX) $(CXXFLAGS) -o assignment4_part3b $(SRCDIR)/assignment4_part3b_3d_texture.cpp $(LDFLAGS)

all: task2_part1 task2_part2 task3_3d_cube task3_part1 assignment4_part1 assignment4_part2 assignment4_part3a assignment4_part3b

clean:
	rm -f red_triangle blue_square task2_part1 task2_part2 task3_3d_cube assignment2_interaction assignment3_3d_cube task3_part1 assignment4_part1 assignment4_part2 assignment4_part3a assignment4_part3b

.PHONY: all clean
