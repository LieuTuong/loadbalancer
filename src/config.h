#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <ctime>
#include <chrono>
#include "loadbalancer.h"
#include "socket.h"
using namespace std;

extern string config_file;

class backend;

void init_log(string ip);

void config_from_file(vector<shared_ptr<backend>> &backends);

char *get_time();


#endif