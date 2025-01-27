CFLAGS := -g -O3 -Wall -Wextra 

lc_sell: lc_sell.c dir
	clang $(CFLAGS) -o build/lc_sell $<

dir:
	mkdir -p build

clean:
	rm -r build
