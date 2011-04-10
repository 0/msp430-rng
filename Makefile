CC = msp430-gcc
CFLAGS = -Wall -Wno-main -Os -g -mmcu=msp430x2211

.PHONY: rand test clean

rand: rand.o

test: test.elf

test.elf: rand.o test.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f test.elf rand.o test.o
