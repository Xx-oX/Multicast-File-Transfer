CC = gcc
CFLAGS = -Wall -I./inc 

INC = ./inc/multicast.h

all: clean server client
	@echo "Completed!!!"

server: ./src/server.c $(INC)
	@echo "Creating $@..."
	$(CC) -o $@ $^ $(CFLAGS)

client: ./src/client.c $(INC)
	@echo "Creating $@..."
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f server client
	@echo "Cleaned!!!"

clear:
	rm -f *_receive
	@echo "Received file cleared!!!"

.PHONY: all clean clear