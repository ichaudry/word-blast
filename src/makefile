all: compile_program run_program

compile_program:
	@gcc main.c hashtable.c -o main -pthread

run_program:
	@./main WarAndPeace.txt 10
