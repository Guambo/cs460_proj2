GL_FLAGS=-lglut -lGLU -lGL
OTHER_FLAGS=-Wall -Wunused -pedantic
FLAGS=$(GL_FLAGS) $(OTHER_FLAGS)

all: proj2

proj2:
	g++ $(FLAGS) proj2.cpp -o a.out

clean:
	rm -f *.o *.out
