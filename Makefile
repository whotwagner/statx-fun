CFLAGS = -Wall

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

statx: statx.o
	$(CC) -o statx $(CFLAGS) statx.o
