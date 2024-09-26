TARGET = main
SOURCES = p1.c list.c
CC = gcc
CFLAGS = -Wall -g

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

run: $(TARGET)
	./$(TARGET)

memcheck: $(TARGET)
	valgrind --tool=memcheck -s --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(TARGET)

