#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <ctime>
#include <chrono>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "loadbalancer.h"
#include "socket.h"
using namespace std;

#define PERMS 0644

struct message {
   long msg_type;
   char msg[100];
};

extern string config_file;
extern int msqid;

class backend;

//void init_log(string ip);

void init_msg_queue();

void config_from_file(vector<shared_ptr<backend>> &backends);

char *get_time();


#endif