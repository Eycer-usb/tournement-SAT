CC = g++
FLAGS = -std=c++11
SRC = src
FILES = ${SRC}/utils.o

all:
	cd ./glucose/simp; make; cd -; mkdir -p bin && cp glucose/simp/glucose bin/glucose;\
	make main

main: ${SRC}/main.cpp ${FILES}
	${CC} ${FLAGS} ${FILES} ${SRC}/main.cpp -o tournement-SAT;\
	rm ${FILES}

utils.o: ${SRC}/utils.cpp
	${CC} -c ${FLAGS} utils.cpp -o ${SRC}/utils.o