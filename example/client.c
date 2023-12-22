#include "server.h"

int client_action(int arg) {
  switch (arg) {
  case 1:
    return server_action("hello world") + 1;
  case 2:
    return server_action("hello world!") + 100;
  default:
    return 123;
  }
}
