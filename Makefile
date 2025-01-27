CFLAGS := -g -O3 -Wall -Wextra 
LDFLAGS := -g -O3 -z noexecstack
HEADERS := cli.h

.PHONY: all
all: dir build/lc_sell

build/lc_sell: build/lc_sell.o build/cli.o
	clang $(LDFLAGS) -o $@ $?

build/lc_sell.o: lc_sell.c $(HEADERS)
	clang -c $(CFLAGS) -o $@ $<

build/cli.o: cli.c $(HEADERS)
	clang -c $(CFLAGS) -o $@ $<

dir:
	mkdir -p build

clean:
	rm -r build
