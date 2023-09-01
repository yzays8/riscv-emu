CC = g++
TARGET = emu
OBJS = main.o cpu.o

CXXFLAGS = -Wall -Wextra -std=c++2b

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf *.o $(TARGET)

.PHONY: run
run:
	./$(TARGET) $(FILE)

$(TARGET): $(OBJS) Makefile
	$(CC) $(OBJS) -o $@

%.o: %.cpp Makefile
	$(CC) $(CXXFLAGS) -c $<
