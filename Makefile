CC = g++
FLAGS = -std=c++11
SRC = src
FILES = bin/utils.o

all:
	cd ./glucose/simp; make; cd -; mkdir -p bin && cp glucose/simp/glucose bin/glucose;\
	make main

main: tournement-SAT

bin:
	mkdir -p bin;

tournement-SAT: ${SRC}/main.cpp ${FILES} bin
	${CC} ${FLAGS} ${FILES} ${SRC}/main.cpp -o tournement-SAT;\


bin/utils.o: ${SRC}/utils.cpp bin
	${CC} -c ${FLAGS} ${SRC}/utils.cpp -o bin/utils.o

result:
	mkdir result

test: tournement-SAT result
	make all >> /dev/null;\
	rm test/test.log 2> /dev/null;\
	ls test/*.json | cut -f2 -d'/' | cut -f1 -d'.' | xargs -I{}  bash -c "./tournement-SAT test/{}.json result/{} >> result/tests.log"

clean:
	rm tournement-SAT;\
	rm -Rfd result tmp bin