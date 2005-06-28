COMPILER = gcc
DFLAGS = -ansi -pedantic -Wall -g
CFLAGS = -c
OFLAGS = -o
EXECNAME = suffixtree

suffixtree:	main.o suffix_tree.o
	${COMPILER} ${DFLAGS} main.o suffix_tree.o ${OFLAGS} ${EXECNAME}

suffix_tree.o:	suffix_tree.c suffix_tree.h
	${COMPILER} ${DFLAGS} ${CFLAGS} suffix_tree.c

main.c:	suffix_tree.h 
	${COMPILER} ${DFLAGS} ${CFLAGS} main.c 

clean: 
	rm *.o 
	rm ${EXECNAME}
