CFLAGS=-Wall -g

debug: CPPFLAGS += -DNDEBUG
debug: CFLAGS += -DNDEBUG
debug: ex01

all: clean ex01 ex03

clean:
	rm -f ex01 ex03
