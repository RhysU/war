CFLAGS=-std=c99 -Wall -Werror -O3
all: data4.csv
war: war.c pcg_basic.c pcg_basic.h
data4.csv: war Makefile
	./$< 0 4 | (head -n1; tail -n1) > "$@"
	for s in `seq 1 10000`; do ./$< $$s 4 | tail -n1 >> "$@"; done
.PHONY: clean
clean:
	rm -f war data4.csv
