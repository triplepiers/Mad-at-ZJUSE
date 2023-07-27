test:
	@rm -f *.bc *.s *.o *.out
	@echo "Preparing for test ..."
	@echo "Generating X86_64 ..."
	@cat ./test.c | ../build/Compiler > ./test.ll
	@llvm-as ./test.ll
	@llc --march=x86-64 ./test.bc 
	@gcc -c test.s
	@gcc test.o -o test.out --static
	@./test.out

ll:
	@rm -f *.bc *.s *.o *.out
	@echo "Generating .ll ..."
	@cat ./test.c | ../build/Compiler > ./test.ll

clean:
	@echo "Cleanning ./test ..."
	@rm -f *.ll *.bc *.s *.o *.out