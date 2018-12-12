CC = gcc
CFLAGS  = -Wall `pkg-config --libs --cflags gtk+-3.0` -l sqlite
TARGET_1 = server
TARGET_2 = main

all: $(TARGET_1) $(TARGET_2)

$(TARGET_1): $(TARGET_1).c
	$(CC) -o $(TARGET_1) $(TARGET_1).c $(CFLAGS)

$(TARGET_2): $(TARGET_2).c
	$(CC) -o $(TARGET_2) $(TARGET_2).c $(CFLAGS)

clean:
	$(RM) $(TARGET_1) $(TARGET_2) *.o a.out