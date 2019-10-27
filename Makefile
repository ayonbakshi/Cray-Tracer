CXX = g++
CXXFLAGS = -std=c++14 -Wall -MMD -g -Ofast
EXEC = main
OBJECTS = main.o Object.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS} ${EXEC}
