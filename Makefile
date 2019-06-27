
PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

DEBUG?= -g -ggdb
CFLAGS?= -O2 -Wall -W -std=c99
LDFLAGS= -lm

# Uncomment the following two lines for coverage testing
#
# CFLAGS+=-fprofile-arcs -ftest-coverage
# LDFLAGS+=-lgcov

all: rax-test rax-oom-test rax-samples

rax-test: rax-test.o rax.o rc4rand.o crc16.o
	$(CC) -o $@ $^ $(LDFLAGS) $(DEBUG)

rax-oom-test: rax-oom-test.o rax.o
	$(CC) -o $@ $^ $(LDFLAGS) $(DEBUG)

rax-samples: rax-samples.o rax.o rc4rand.o
	$(CC) -o $@ $^ $(LDFLAGS) $(DEBUG)

%.o: $(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(DEBUG) -o $@ $<

clean:
	rm -f rax-test rax-oom-test *.gcda *.gcov *.gcno *.o

