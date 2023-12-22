// WARNING: auto-generated file - Changes will be lost!
#include "server.h"

#define weak_mock __attribute__((weak))

// function: server_action
typedef int (*mockoto_server_action_f) (const char *);
static mockoto_server_action_f _server_action_hook;
void mockoto_server_action_hook(mockoto_server_action_f cb)
{
	_server_action_hook = cb;
}
static int _server_action_retval;
void mockoto_server_action_returns(int ret)
{
	_server_action_retval = ret;
}
static int _server_action_count;
int mockoto_server_action_called(void)
{
	int count = _server_action_count;
	_server_action_count = 0;
	return count;
}
weak_mock int
server_action(const char * __param_0)
{
	_server_action_count++;
	if(_server_action_hook)
		return _server_action_hook(__param_0);
	return _server_action_retval;
}
