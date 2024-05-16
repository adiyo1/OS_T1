CC=gcc

all: add2PB findPhone

add2PB: add2PB.c
	$(CC) -o add2PB add2PB.c

findPhone: findPhone.c
	$(CC) -o findPhone findPhone.c

clean:
	rm -f add2PB findPhone
