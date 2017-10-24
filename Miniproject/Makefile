TARGET = tainyproject

# compiler flags
CFLAGS = -g -Wall 

# linker flags
LDFLAGS = -g -lrt -pthread


# list of sources
SOURCES = $(shell find -name "*.c")


# default rule, to compile everything
all: $(TARGET)


# define object files
OBJECTS = $(SOURCES:.c=.o)


# compile
%.o : %.c
	/home/student/Desktop/EXC8_t/buildroot-avr32-v3.0.0_TTK4147/output/staging/usr/bin/avr32-linux-gcc $(CFLAGS) -c -o $@ $<


# link programs
$(TARGET): $(OBJECTS)
	/home/student/Desktop/EXC8_t/buildroot-avr32-v3.0.0_TTK4147/output/staging/usr/bin/avr32-linux-gcc -o $@ $^ $(LDFLAGS)


# cleaning
clean:
	rm -f $(TARGET) $(OBJECTS) 
