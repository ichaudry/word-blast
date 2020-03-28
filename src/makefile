all: compile_program run_program run_program_2 run_program_3 run_program_4

compile_program:
	@gcc main.c hashtable.c -o main -pthread

run_program:
	@./main WarAndPeace.txt 1

run_program_2:
	@./main WarAndPeace.txt 2

run_program_3:
	@./main WarAndPeace.txt 4

run_program_4:
	@./main WarAndPeace.txt 8