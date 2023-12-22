#include "client.h"
#include "server.h"
#include "server_mock.h"
#include <assert.h>
#include <string.h>

// -----------------------------------------------------------------------------
// TEST: call interfaces without setting any special hook
// -----------------------------------------------------------------------------
void test1(void) {
  // without any mock hooks, return of server_action will be 0
  assert(server_action(NULL) == 0);
  assert(server_action("test") == 0);

  // following the client code, we know what to expect
  assert(client_action(1) == 1);
  assert(client_action(2) == 100);
  assert(client_action(4) == 123);
}

// -----------------------------------------------------------------------------
// TEST: call interfaces without setting a hook
// -----------------------------------------------------------------------------
static int my_hook(const char *msg) {
  return (strcmp(msg, "hello world") == 0) ? 1000 : 0;
}
void test2(void) {
  // set hook for function server_action
  mockoto_server_action_hook(my_hook);

  // calling the interface directly
  assert(server_action("hello world") == 1000);
  assert(server_action("hello world!!") == 0);

  // following the client code, we know what to expect
  assert(client_action(1) == 1001);
  assert(client_action(2) == 100);
  assert(client_action(4) == 123);

  // unset hook
  mockoto_server_action_hook(NULL);
}

// -----------------------------------------------------------------------------
// TEST: count how often mock functions are called
// -----------------------------------------------------------------------------
void test3(void) {
  // reset previous counts
  (void)mockoto_server_action_called();

  // perform same calls
  assert(server_action(NULL) == 0);
  assert(server_action("test") == 0);
  assert(client_action(1) == 1);
  assert(client_action(2) == 100);
  assert(client_action(4) == 123);

  // 2 direct calls, 2 calls from client_action (with parameters 1 and 2)
  assert(mockoto_server_action_called() == 4);
}

// -----------------------------------------------------------------------------
// TEST: return a specific value from mock function
// -----------------------------------------------------------------------------
void test4(void) {
  mockoto_server_action_returns(1000);
  assert(server_action(NULL) == 1000);
  assert(server_action("test") == 1000);

  mockoto_server_action_returns(2);
  assert(client_action(1) == 3);

  mockoto_server_action_returns(100);
  assert(client_action(2) == 200);

  // client_actiong does not call server_action here
  assert(client_action(4) == 123);
}

// -----------------------------------------------------------------------------
// main function
// -----------------------------------------------------------------------------
int main(void) {
  test1();
  test2();
  test3();
  test4();
  return 0;
}
