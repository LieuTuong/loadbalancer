#ifndef __HEADER_H__
#define __HEADER_H__
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
using namespace std;

struct message {
   long msg_type;
   char msg[100];
};


void send_mess(std::string ip, int port, std::string msg);


#endif