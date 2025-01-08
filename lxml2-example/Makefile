PROG = lxml2-example
CFLAGS += -g -O2 -Wall
CFLAGS += -std=gnu17

LDLIBS += -lxml2 -lz
# LDLIBS += -lxml2
# LDFLAGS += -Wl,--copy-dt-needed-entries

all: $(PROG)

clean:
	rm -f *.o $(PROG)
