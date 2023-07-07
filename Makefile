CC= g++
FLAGS= -ljsoncpp

all:
	cd ./glucose/simp; make; cd -; mkdir -p bin && cp glucose/simp/glucose bin/glucose

main: main.cpp
	${CC} ${FLAGS} main.cpp -o tournement-SAT
