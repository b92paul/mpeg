CFLAGS = -O3 -Wall -Wextra --std=c++11 -framework OpenGL -framework GLUT -framework Foundation -pthread -Wno-deprecated-declarations
LIB=-L lib
INC = -I include
test/%.o: test/%.cpp include/*
	g++ -o $@ $< ${CFLAGS} ${INC}
bin/%.o:	src/%.cpp include/*
	g++ -o $@ $< ${CFLAGS} ${INC}
