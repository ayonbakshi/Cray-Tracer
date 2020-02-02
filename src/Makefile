CXX = g++
CXXFLAGS = -std=c++14 -Wall -MMD -g -Ofast -fopenmp
EXEC = main
OBJECTS = main.o Object.o KDTree.o Raycaster.o Material.o Camera.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} -fopenmp -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS} ${EXEC}
