// WARNING: auto-generated file - Changes will be lost!
#ifndef MOCKOTO__H
#define MOCKOTO__H

#include "server.h"

// function: server_action
typedef int (*mockoto_server_action_f) (const char *);
void mockoto_server_action_hook(mockoto_server_action_f cb);
void mockoto_server_action_returns(int ret);
int mockoto_server_action_called(void);

#endif
