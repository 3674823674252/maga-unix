all: build

build: server.c client.c
	rm -f httpserver
	gcc -W -Wall -pthread -o httpserver server.c -lm
	gcc -W -Wall -o httpclient client.c

clean:
	rm httpserver
