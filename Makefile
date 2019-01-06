CFLAGS=-std=c99 -Wall -Werror -O3
ROUNDS=10000
all: data4.csv.gz

war: war.c pcg_basic.c pcg_basic.h
data4.csv: war Makefile
	./$< 0 4 | (head -n1; tail -n1) > "$@"
	for s in `seq 1 $(ROUNDS)`; do ./$< $$s 4 | tail -n1 >> "$@"; done
data4.csv.gz: data4.csv
	gzip -f "$<" > $@

.PHONY: clean
clean:
	rm -f war data4.csv data4.csv.gz
