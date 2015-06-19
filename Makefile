CFLAGS = -O3 -Wall -Wextra
LIB=-L lib
INC = -I include
test/%.o: test/%.cpp include/*
	g++ -o $@ $< ${CFLAGS} ${INC}
bin/%.o:	src/%.cpp include/*
	g++ -c -o $@ $^ ${CFLAGS} ${INC}
