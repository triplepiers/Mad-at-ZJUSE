# lib/Makefile
SRC = printk.c rand.c string.c
OBJ = printk.o rand.o string.o

all : $(OBJ)
%.o : %.c
	${GCC} ${CFLAG} -c $(SRC)

.PHONY : clean
clean:
	-rm $(OBJ)