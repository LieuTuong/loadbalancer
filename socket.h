#ifndef __SOCKET_H__
#define __SOCKET_H__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define MAX_CLIENTS 100
extern string ERR_MSG;
extern vector<pollfd> poll_sets;


int Socket();
void Setsockopt(int lsockfd);
void Bind(int lsockfd, string ip, int port);
void Listen(int lsockfd);
int Accept(int lsockfd);
sockaddr_in get_client_addr(int sockfd);

void server_overwheled(int client_sock);
void print_start(string ip, int port);
void add_client(int client_sock);
void del_client(vector<pollfd>::iterator it);
#endif