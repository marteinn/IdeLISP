build:
	cc -std=c99 -Wall idelisp.c mpc.c -o ./bin/idelisp -ledit -lm

build_wasm:
	emcc -o wasm/idelisp.js idelisp_wasm.c mpc.c -O3 -s WASM=1 -s NO_EXIT_RUNTIME=1  -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']"

run:
	./bin/idelisp

debug:
	lldb ./bin/idelisp

test:
	./bin/idelisp -f tests.ilisp
