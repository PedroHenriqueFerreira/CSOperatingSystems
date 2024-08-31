CC = gcc

CFLAGS = -Wall -Wextra -std=c99 -lm

TARGET = main

all: $(TARGET) run_all

$(TARGET): main.c
	$(CC) main.c $(CFLAGS) -o $(TARGET)

run_all: $(TARGET)
	for i in 1 2 3 4 5; do \
		echo "----- sistema$$i.txt -----"; \
		./$(TARGET) SistemasTestes/sistema$$i.txt; \
	done; \

clean:
	rm -f $(TARGET)

.PHONY: all clean run_all