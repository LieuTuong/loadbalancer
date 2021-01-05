#ifndef __LOG_H__
#define __LOG_H__


#include "loadbalancer.h"
#include <fstream>

#define RESET "\033[0m"
#define BLUE "\033[34m"
#define RED "\033[31m"    /* Red */
#define GREEN "\033[32m"  /* Green */
#define YELLOW "\033[33m" /* Yellow */
#define CYAN "\033[36m"

static ofstream logfd;

void log_init(string filename, string ip);

string log(string time, string ip, int port);

void log( string msg);

void log_terminal(string time, string ip, int port);

void log_close();



#endif