all:
	@echo "Working in ./src ..."
	@flex -o lex.yy.cpp lex.l
	@bison -d -o parse.cpp parse.y
	
clean: 
	@echo "Cleanning ./src ..."
	@rm -f lex.yy.cpp parse.hpp parse.cpp