COMPILER = g++
DFLAGS = -ansi -pedantic -Wall -g
CFLAGS = -c
OFLAGS = -o
EXECNAME = suffixtree

suffixtree:	main.o suffix_tree.o
	${COMPILER} ${DFLAGS} main.o suffix_tree.o ${OFLAGS} ${EXECNAME}

suffix_tree.o:	suffix_tree.cpp suffix_tree.h
	${COMPILER} ${DFLAGS} ${CFLAGS} suffix_tree.cpp

main.cpp:	suffix_tree.h 
	${COMPILER} ${DFLAGS} ${CFLAGS} main.cpp

clean: 
	rm *.o 
	rm ${EXECNAME}
