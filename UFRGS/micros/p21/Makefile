TARGET=p21
SRCS=$(TARGET).c

# Using hardware trigger
FLAGS=-Wall -g -MMD -lm

# Using software trigger
#FLAGS=-Wall -g -MMD -DTRIG_SYSFS

INCLUDE=-I. -I../lib
LIBDIR=-L../lib
LIBS=-lgalileo2

CC=$(CROSS_COMPILE)gcc
CFLAGS=$(FLAGS) $(INCLUDE)
LDFLAGS=$(LIBDIR) $(LIBS) -lm

all: $(TARGET)

$(TARGET): $(SRCS:.c=.o) ../lib/libgalileo2.a
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(SRCS:.c=.d)

clean:
	rm -f *~ *.bak *.o *.d

distclean: clean
	rm -f $(TARGET)
