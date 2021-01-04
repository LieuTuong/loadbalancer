#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#define BUF_SIZE 100
using namespace std;

const char SERVER_IP[] = "127.0.0.1";

int main(int argc, char *argv[])
{
    //Create socket
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("could not create socket");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(5555);

    //Connect to remote server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect failed");
        exit(1);
    }

    //keep communicating with server
    int count = 0;
  
       string msg;
       cin>>msg;

        //Send count

        if (send(sock, msg.c_str(), msg.length(), 0) < 0)
        {
            perror("send failed");
            exit(1);
        }

        /*//Receive a reply from the server
        if (recv(sock, message, BUF_SIZE, 0) < 0)
        {
            perror("recv failed");
            exit(1);
        }
        else
        {
            cout << message << endl;
        }*/
    

    close(sock);
    exit(0);
}
