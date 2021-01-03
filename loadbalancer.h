#ifndef __LOADBALANCER_H__
#define __LOADBALANCER_H__
#include "socket.h"
#include <string.h>
extern vector<pollfd> poll_sets;
extern uint BUF_SIZE;
void start_server(string ip, int port);

#endif