CFLAGS=-std=c99 -Wall -Werror -O3
ROUNDS=10

all: data.csv.gz

war: war.c pcg_basic.c pcg_basic.h

data.csv.gz: data1.csv.gz data2.csv.gz data3.csv.gz data4.csv.gz data5.csv.gz data6.csv.gz
	zcat $^ | awk 'NR == 1 || !/^pcg32seq/' | gzip -f > $@

data1.csv: war Makefile
	./$< 0 1 | (head -n1; tail -n1) > "$@"
	for s in `seq 1 $(ROUNDS)`; do ./$< $$s 1 | tail -n1 >> "$@"; done
data1.csv.gz: data1.csv
	gzip -f < $< > $@

data2.csv: war Makefile
	./$< 0 2 | (head -n1; tail -n1) > "$@"
	for s in `seq 1 $(ROUNDS)`; do ./$< $$s 2 | tail -n1 >> "$@"; done
data2.csv.gz: data2.csv
	gzip -f < $< > $@

data3.csv: war Makefile
	./$< 0 3 | (head -n1; tail -n1) > "$@"
	for s in `seq 1 $(ROUNDS)`; do ./$< $$s 3 | tail -n1 >> "$@"; done
data3.csv.gz: data3.csv
	gzip -f < $< > $@

data4.csv: war Makefile
	./$< 0 4 | (head -n1; tail -n1) > "$@"
	for s in `seq 1 $(ROUNDS)`; do ./$< $$s 4 | tail -n1 >> "$@"; done
data4.csv.gz: data4.csv
	gzip -f < $< > $@

data5.csv: war Makefile
	./$< 0 5 | (head -n1; tail -n1) > "$@"
	for s in `seq 1 $(ROUNDS)`; do ./$< $$s 5 | tail -n1 >> "$@"; done
data5.csv.gz: data5.csv
	gzip -f < $< > $@

data6.csv: war Makefile
	./$< 0 6 | (head -n1; tail -n1) > "$@"
	for s in `seq 1 $(ROUNDS)`; do ./$< $$s 6 | tail -n1 >> "$@"; done
data6.csv.gz: data6.csv
	gzip -f < $< > $@

.PHONY: clean
clean:
	rm -f war data*.csv data*.csv.gz
