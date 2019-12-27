build:
	cc -std=c99 -Wall idelisp.c mpc.c -o ./bin/idelisp -ledit && ./bin/idelisp

debug:
	lldb ./bin/idelisp
