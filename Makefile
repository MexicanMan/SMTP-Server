C_VER := -std=c99
FLAGS := -g3 -gdwarf-2 -DDEBUG -g

all: server client

server: server.o shared.o
	gcc $(C_VER) $(FLAGS) -o server build/server.o build/shared.o

server.o: SMTPServer/server.c SMTPShared/full_io.h directories
	gcc $(C_VER) $(FLAGS) -c SMTPServer/server.c -o build/server.o

client: client.o shared.o
	gcc $(C_VER) $(FLAGS) -o client build/client.o build/shared.o

client.o: SMTPClient/client.c SMTPShared/full_io.h directories
	gcc $(C_VER) $(FLAGS) -c SMTPClient/client.c -o build/client.o

shared.o: SMTPShared/full_io.h SMTPShared/full_io.c directories
	gcc $(C_VER) $(FLAGS) -c SMTPShared/full_io.c -o build/shared.o

directories:
	mkdir build

clean:
	rm -rf build/*.o server client