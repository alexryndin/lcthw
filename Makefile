CFLAGS=-Wall -g

debug: CPPFLAGS += -DNDEBUG
debug: CFLAGS += -DNDEBUG
debug: ex01

ex22.o: ex22.c

ex22: ex22_main.c ex22.o

all: clean ex01 ex03

clean:
	rm -f ex01 ex03
