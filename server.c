// Retyped from david's server
#include <stdio.h>
#include <math.h>
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

#define SOCK_CREATE 85
#define SOCK_BIND 86
#define SOCK_CONNECT 87
#define SOCK_LISTEN 88

#define CLIENT_THREAD 89

#define PORT 3000

#define issp(x) isspace((int)x)
#define sockaddr(x) (struct sockaddr *)&x
#define int(x) (int)x

void write_headers(int client_socket, char * filename) {
  char buf[1024];
  struct stat st;

  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  send(client_socket, buf, strlen(buf), 0);

  stat(filename, &st);
  sprintf(buf, "Content-Length: %zd\r\n\r\n", st.st_size);
  printf("Content-Length: %s", buf);
  send(client_socket, buf, strlen(buf), 0);
}

void send_file(int client_socket, FILE* file) {
  char buf[1024];

  fgets(buf, sizeof(buf), file);
  while(!feof(file)) {
    printf("Sending: '%s'", buf);
    send(client_socket, buf, strlen(buf), 0);
    fgets(buf, sizeof(buf), file);
  }
}

int get_line(int socket, char *buf, int size) {
  int i = 0;
  char c = '\0';
  int n;

  while ((i < size - 1) && c != '\n') {
    n = recv(socket, &c, 1, 0);

    if (n > 0) {
      if (c == '\r') {
        n = recv(socket, &c, 1, MSG_PEEK);
        if ((n > 0) && c == '\n') {
          recv(socket, &c, 1, 0);
        } else {
          c = '\n';
        }
      }
      buf[i] = c;
      i++;
    } else {
      c = '\n';
    }
  }

  buf[i] = '\0';

  return i;
}

void do_404(int client_socket) {
  char buf[1024];

  strcpy(buf, "HTTP/1.0 404 Not Found\r\n\r\n");
  send(client_socket, buf, strlen(buf), 0);
}

void serve(int client_socket, char * filename) {
 FILE* file = NULL;

 file = fopen(filename, "r");
 if (file == NULL) {
  do_404(client_socket);
 } else {
  write_headers(client_socket, filename);
  send_file(client_socket, file);
 }

 fclose(file);
}

void process_request(int client_socket) {
  char buf[1024];
  int numchars;
  char method[255];
  char url[512];
  char path[512];

  size_t i, j;
  struct stat st;

  numchars = get_line(client_socket, buf, sizeof(buf));
  i = 0; j = 0;

  while (!issp(buf[j]) && (i < sizeof(method) - 1)) {
    method[i] = buf[j];
    i++; j++;
  }

  method[i] = '\0';

  printf("HTTP method: %s\n", method);

  while(issp(buf[j]) && j < sizeof(buf)) {
    j++;
  }

  i = 0;

  while (!issp(buf[j]) && (i < sizeof(url) - 1) && j < sizeof(buf)) {
    url[i] = buf[j];
    i++; j++;
  }

  url[i] = '\0';

  printf("Path requested: %s\n", url);

  sprintf(path, "static%s", url);

  while ((numchars > 0) && strcmp("\n", buf)) {
    numchars = get_line(client_socket, buf, sizeof(buf));
  }

  if (stat(path, &st) == -1) {
    printf("404\n");
    do_404(client_socket);
  } else {
    printf("200\n");
    serve(client_socket, path);
  }

  fflush(stdout);
  close(client_socket);
}

int main(void) {
  int server_socket = -1;
  int client_socket = -1;
  struct sockaddr_in server_name;
  struct sockaddr_in client_name;
  int server_name_len;
  int client_name_len;

  pthread_t client_thread;

  printf("Initializing server..\n");
  server_socket = socket(PF_INET, SOCK_STREAM, 0);

  if (server_socket == -1) {
    exit(SOCK_CREATE);
  }

  memset(&server_name, 0, sizeof(server_name));
  server_name.sin_family = AF_INET;
  server_name.sin_port = htons(PORT);
  server_name.sin_addr.s_addr = htonl(INADDR_ANY);

  printf("Server is running on :3000. Server socket: %d\n", server_socket);
  fflush(stdout);

  server_name_len = sizeof(server_name);
  client_name_len = sizeof(client_name);

  if (bind(server_socket, sockaddr(server_name), server_name_len) == -1) {
    exit(SOCK_BIND);
  }

  if (listen(server_socket, 5) < 0) {
    exit(SOCK_LISTEN);
  }

  while (1) {
    client_socket = accept(server_socket, sockaddr(client_name), &client_name_len);
    if (client_socket == -1) {
      exit(SOCK_CONNECT);
    }

    printf("Incoming connection from %d.%d.%d.%d\n", 
      int(client_name.sin_addr.s_addr&0xFF),
      int((client_name.sin_addr.s_addr&0xFF00)>>8),
      int((client_name.sin_addr.s_addr&0xFF0000)>>16),
      int((client_name.sin_addr.s_addr&0xFF000000)>>24));

    printf("Processing connection in a separate thread..\n");
    if (pthread_create(&client_thread, NULL, process_request, client_socket) != 0) {
      exit(CLIENT_THREAD);
    }
  }

  close(server_socket);

  return 0;
}