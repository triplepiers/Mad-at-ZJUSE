# lib/Makefile
SRC = printk.c
OBJ = printk.o

all : $(OBJ)
%.o : %.c
	${GCC} ${CFLAG} -c $(SRC)

.PHONY : clean
clean:
	-rm $(OBJ)