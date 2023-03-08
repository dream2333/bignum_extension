LIBSRCS=bn.c
LIBOBJS=bn.o
LIB=libbn.a
CALCSRCS=calc.c
CALCOBJS=calc.o
CALC=calc
CFLAGS= -g -Wall
LDFLAGS=-g
LDLIBS=${LIB}


all: ${LIB} ${CALC}

${CALC}: ${CALCOBJS} ${LIB}
	${CC} -o ${CALC} ${LDFLAGS} ${CALCOBJS} ${LDLIBS}

${LIB}: ${LIBOBJS}
	${AR} rcs ${LIB} ${LIBOBJS}

bn.o: bn.h

calc.o: bn.h


clean:
	rm -f ${CALC} ${CALCOBJS} ${LIB} ${LIBOBJS}
