build:
	cc -std=c99 -Wall idelisp.c mpc.c -o ./bin/idelisp -ledit -lm

run:
	./bin/idelisp

debug:
	lldb ./bin/idelisp

test:
	./bin/idelisp -f tests.ilisp
