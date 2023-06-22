#ifndef __SERVER_HH__
#define __SERVER_HH__

#include <sys/socket.h>

#define EVENT_TYPES_LEN 1
#define BUF_SIZE 1024
#define MAX_EVENTS 10

typedef struct Request {
  struct sockaddr client_addr;
  socklen_t addr_size;
  int server_sockfd;
  char *msg;
} Request;

typedef struct Server {
  int port;
  int sockfd;
  void (*cb_array[EVENT_TYPES_LEN]) (Request *);
} Server;

typedef enum EventType { EventTypeMessage } EventType;

Server *createServer(int port);
void on(Server *server, EventType event, void (*callback)(Request *));
void startServer(Server *server);

#endif
