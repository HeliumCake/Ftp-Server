server: server.c handle.c server.h handle.h
	gcc server.c handle.c -o server -lpthread -Wall

clean:
	rm server