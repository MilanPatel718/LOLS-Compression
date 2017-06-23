PROGRAMS = compressR_LOLS compressR_worker_LOLS compressT_LOLS

CC = gcc
CFLAGS = -Wall -g -lm -pthread

%: %.c %.h
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: all clean

all: $(PROGRAMS)

clean:
	@rm -f *.o $(PROGRAMS)
