CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -lm

TARGET = main

PYTHON_SCRIPT = plot_results.py

all: $(TARGET) run_all

$(TARGET): main.c
	$(CC) main.c $(CFLAGS) -o $(TARGET)

run_all: $(TARGET)
	for i in 1 2 3 4 5 6; do \
		./$(TARGET) SistemasTestes/sistema$$i.txt Output/sistema$$i.csv; \
	done; \

clean:
	rm -f $(TARGET)

.PHONY: all clean run_all
