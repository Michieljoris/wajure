src = $(wildcard src/*.c)
tmp = $(subst src,out,$(src))
objs = $(tmp:.c=.o)
deps = $(objs:.o=.d)


# -D WASM
# -Wl,-error-limit=0
# https://clang.llvm.org/docs/CommandGuide/clang.html
CC = clang
CFLAGS_COMMON = -g -std=c99 -Wall -flto -Isrc/include -Ilib/include

X86FLAGS = -pthread $(CFLAGS_COMMON)
X86LDFLAGS =  -Lout -Llib  -ledit -lbinaryen

WASMFLAGS = $(CFLAGS_COMMON) --target=wasm32 -O3  -flto -nostdlib -D WASM
# https://lld.llvm.org/WebAssembly.html
WASMLDFLAGS = -Lout -Wl,--no-entry \
-Wl,--export-all \
-Wl,--allow-undefined  \
-Wl,--lto-O3
# ,-allow-undefined-file wasm.syms
#,--export-dynamic  \

ifeq ($(PLATFORM),wasm)
	BUILD_ARTIFACT = out/lispy.wasm
	CFLAGS = $(WASMFLAGS)
	LDFLAGS = $(WASMLDFLAGS)
	EXEC =
else
	BUILD_ARTIFACT = out/lispy
	CFLAGS = $(X86FLAGS)
	LDFLAGS = $(X86LDFLAGS)
	EXEC = out/lispy lispy/repl.lispy
endif


all: $(BUILD_ARTIFACT)
	$(EXEC)

# c files to dep files, if c file changes, rebuild its dep file
out/%.d : src/%.c
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS_COMMON) $< -o $@

# insert those dep files here (.o: list of deps (..h files) )
# so if any of the deps change, mark the .o file for rebuild
-include $(deps)

# c files to object files
# rebuild the .o file it its .c file changes or is marked.
out/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@


# link object files, rebuild if any of the .o files changed
$(BUILD_ARTIFACT): $(objs)
	$(CC) $(CFLAGS) $^ $(LDFLAGS)  -o $(BUILD_ARTIFACT)


clean:
	rm -f $(objs) $(deps) $(BUILD_ARTIFACT)
