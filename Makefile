CC = msp430-gcc
CFLAGS = -Wall -Wno-main -Os -g -mmcu=msp430x2211

OBJS=rand.o

all: $(OBJS)
	$(CC) $(CFLAGS) -o main.elf $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f main.elf $(OBJS)
