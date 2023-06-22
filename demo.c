#include <stdio.h>

#include "server.h"

void handleMessage(Request *req) {
  printf("msg=%s", req->msg);
}

int main() {
  Server *s = createServer(19200);

  on(s, EventTypeMessage, handleMessage);

  startServer(s);
}
