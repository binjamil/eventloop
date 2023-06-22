#include <stdio.h>
#include <stdlib.h>

#include "server.h"

void handleMessage(Request *req) {
  if (sendto(req->server_sockfd, req->msg, BUF_SIZE, 0, &req->client_addr, req->addr_size) == -1) {
    perror("send");
    exit(EXIT_FAILURE);
  }
}

int main() {
  Server *s = createServer(41234);

  on(s, EventTypeMessage, handleMessage);

  startServer(s);
}
