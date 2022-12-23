# lib/Makefile
SRC = print.c
OBJ = print.o

all : $(OBJ)
%.o : %.c
	${GCC} ${CFLAG} -c $(SRC)

.PHONY : clean
clean:
	-rm $(OBJ)