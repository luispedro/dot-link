COMPILER = g++
DFLAGS = -ansi -pedantic -Wall -g
CXXFLAGS = -c -I/sw/include/ -Wno-long-double
OFLAGS = -o
EXECNAME = suffixtree

suffixtree: main.o suffix_tree.o
	${COMPILER} ${DFLAGS} main.o suffix_tree.o ${OFLAGS} ${EXECNAME}

suffix_tree.o:	suffix_tree.cpp suffix_tree.h
	${COMPILER} ${DFLAGS} ${CXXFLAGS} suffix_tree.cpp

main.o:	suffix_tree.h 
	${COMPILER} ${DFLAGS} ${CXXFLAGS} main.cpp

clean: 
	rm *.o 
	rm ${EXECNAME}
