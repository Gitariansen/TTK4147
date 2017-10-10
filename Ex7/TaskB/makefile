TARGET = assignmentB

XENO_DESTDIR:=
XENO_CONFIG:=$(XENO_DESTDIR)/usr/bin/xeno-config
XENO_CFLAGS:=$(shell DESTDIR=$(XENO_DESTDIR) $(XENO_CONFIG) --skin native --cflags)
XENO_LIBS:=$(shell DESTDIR=$(XENO_DESTDIR) $(XENO_CONFIG) --skin native --ldflags)

# compiler flags
CFLAGS = -g -Wall -D_GNU_SOURCE $(XENO_CFLAGS) $(XENO_LIBS)


# linker flags
LDFLAGS = -g -lrt -pthread -lcomedi -lm $(XENO_CFLAGS) $(XENO_LIBS)


# list of sources
SOURCES = $(shell find -name "*.c")


# default rule, to compile everything
all: $(TARGET)


# define object files
OBJECTS = $(SOURCES:.c=.o)


# compile
%.o : %.c
	gcc $(CFLAGS) -c -o $@ $<


# link programs
$(TARGET): $(OBJECTS)
	gcc -o $@ $^ $(LDFLAGS)


# cleaning
clean:
	rm -f $(TARGET) $(OBJECTS) 
