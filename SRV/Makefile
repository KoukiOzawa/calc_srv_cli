CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
			$(CC) -o server $(OBJS) $(LDFLAGS)

$(OBJS): calc.h

clean:
		rm -f 9cc *.o *~ tmp*

.PHONY: test clean
