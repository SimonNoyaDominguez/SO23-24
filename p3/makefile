TARGET = main
SOURCES = p3.c list.c
OBJECTS = p3.o list.o
CC = gcc
CFLAGS = -Wall -g

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

p2.o: p3.c list.h
	$(CC) -c p3.c

list.o: list.c list.h
	$(CC) -c list.c

run: $(TARGET)
	./$(TARGET)

memcheck: $(TARGET)
	valgrind --tool=memcheck -s --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)

