#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

#include "server.h"

Server *createServer(int port) {
  char port_str[10];
  sprintf(port_str, "%d", port);
  int sock;
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;      // IPv4
  hints.ai_socktype = SOCK_DGRAM; // UDP datagram
  hints.ai_flags = AI_PASSIVE;    // fill in my IP for me
  if (getaddrinfo(NULL, port_str, &hints, &res) != 0) {
    perror("getaddrinfo");
    exit(EXIT_FAILURE);
  }
  sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sock == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  if (bind(sock, res->ai_addr, res->ai_addrlen) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }
  freeaddrinfo(res);
  Server *server = malloc(sizeof(Server));
  memset(server, 0, sizeof(Server));
  server->port = port;
  server->sockfd = sock;
  return server;
}

void on(Server *server, EventType event, void (*callback)(Request *)) {
  server->cb_array[event] = callback;
}

void startServer(Server *server) {
  struct epoll_event ev, events[MAX_EVENTS];
  int epollfd, nfds;
  int sock = server->sockfd;

  epollfd = epoll_create1(0);
  if (epollfd == -1) {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }

  ev.events = EPOLLIN;
  ev.data.fd = sock;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &ev) == -1) {
    perror("epoll_ctl: sockfd");
    exit(EXIT_FAILURE);
  }

  printf("Listening on port %d\n", server->port);

  // The event loop
  while (1) {
    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1) {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nfds; i++) {
      if (events[i].events & EPOLLIN && events[i].data.fd == sock) {
        struct sockaddr client_addr;
        socklen_t addr_size = sizeof(client_addr);
        char buf[BUF_SIZE];
        memset(buf, 0, BUF_SIZE);
        int bytes_read;
        bytes_read = recvfrom(sock, buf, BUF_SIZE, 0, &client_addr, &addr_size);
        if (bytes_read == -1) {
          perror("recv");
          exit(EXIT_FAILURE);
        }
        // Run the callback
        Request req = {client_addr, addr_size, sock, buf};
        server->cb_array[EventTypeMessage](&req);
      }
    }
  }
}
