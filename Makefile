CC = msp430-elf-gcc
CFLAGS = -Wall -Wextra -Wno-main -Os -g -mmcu=msp430g2211 -specs=nosys.specs

.PHONY: rand test clean

rand: rand.o

test: test.elf

test.elf: rand.o test.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f test.elf rand.o test.o
