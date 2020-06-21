# lispy:
# 	gcc -std=c99 -Wall -Isrc/include -Ilib/include -Lout src/*.c -ledit -lmpc -o out/lispy
# mpc:
# 	gcc -std=c99 -Wall -Ilib/include -c lib/mpc.c -o out/mpc.o
# 	ar rcs out/libmpc.a out/mpc.o


src = $(wildcard src/*.c)
tmp = $(subst src,out,$(src))
objs = $(tmp:.c=.o)
deps = $(objs:.o=.d)

# -D WASM
CC = gcc
CFLAGS = -pthread -g -std=c99 -Wall -Isrc/include -Ilib/include
LDFLAGS = -Lout -Llib -Wl,-rpath=./lib -ledit -lbinaryen

all: out/lispy
	out/lispy lispy/repl.lispy

# out/lispy lispy/stdlib.lispy lispy/test.lispy

-include $(deps)

# c files to object files
out/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# c files to dep files
out/%.d : src/%.c
	$(CC) -MM -MT $@ $(CFLAGS) $< -o $@

# link object files
out/lispy: $(objs)
	$(CC) $(CFLAGS) $^ $(LDFLAGS)  -o $@

clean:
	rm -f $(objs) $(deps) out/lispy

