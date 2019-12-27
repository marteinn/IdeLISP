build:
	cc -std=c99 -Wall idelisp.c mpc.c -o ./bin/idelisp -ledit

run:
	./bin/idelisp

debug:
	lldb ./bin/idelisp
