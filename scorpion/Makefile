CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -g #-fsanitize=address
INCLUDES = -Iinclude -Ilib
LIBS =

SRCS = $(wildcard src/*.cpp)

OBJS = $(SRCS:.cpp=.o)

MAIN = scoc-65816

.PHONY: clean

all:    $(MAIN)
		@echo  Scorpion built successfully.

$(MAIN): $(OBJS)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)
.cpp.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
		$(RM) *.o *~ $(MAIN)
		find src/ -name "*.o" -type f -delete

run:    ${MAIN}
	./scoc-65816