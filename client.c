// Retyped from david's server
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>

#define WRONG_INVOCATION 85
#define SOCKET_CONNECT 86
#define SOCKET_SEND 87

#define sockaddr(x) (struct sockaddr *)&x

int main(int argc, char* argv[]) {
  int client_socket;
  struct sockaddr_in address;
  int address_len;
  int bytes_read;

  char buf[1024];

  if (argc != 2) {
    printf("Syntax: client [/path]");
    exit(WRONG_INVOCATION);
  }

  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons(3000);

  address_len = sizeof(address);

  if (connect(client_socket, sockaddr(address), address_len) == -1) {
    exit(SOCKET_CONNECT);
  }

  sprintf(buf, "GET %s HTTP/1.0\r\n\r\n", argv[1]);

  if (send(client_socket, buf, strlen(buf), 0) < 0) {
    exit(SOCKET_SEND);
  }

  do {
    memset(buf, sizeof(buf), 0);
    bytes_read = recv(client_socket, buf, strlen(buf), 0);
    if (bytes_read > 0) {
      printf("%s", buf);
    }
  } while (bytes_read > 0);

  close(client_socket);
  return 0;
}