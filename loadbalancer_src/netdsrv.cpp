#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <csignal>
#include<cstdlib>
#include <fstream>
using namespace std;
#define PORT 11111
#define LOG_DIR "/loadbalancer/loadbalancer_logs/"

string lb_ip="172.18.0.3";
int msg_id;
ofstream logfd;

struct message
{
    long msg_type;
    char msg[100];
};

void log_init()
{
    string fname(LOG_DIR);
    fname.append("log.txt");
    logfd.open(fname, fstream::app);
    if (logfd.is_open())
    {
        logfd << "===================================" << endl;
        logfd << " Server's LOGS FILE " << endl;
        logfd << "===================================\n\n\n";
    }
    logfd.close();
}

void log_open()
{
    string fname(LOG_DIR);
    fname.append("log.txt");
    logfd.open(fname, fstream::app);
}

void log_close()
{
    logfd.close();
}

void log(string msg)
{
    logfd << msg;
}

void sigStop_handler(int sig)
{
    msgctl(msg_id, IPC_RMID, NULL);
    exit(0);
}

int main()
{
/*    key_t my_key=333;
    msg_id = msgget(my_key, 0666 | IPC_CREAT);*/
log_init();
   // signal(SIGINT, sigStop_handler);

    //nhan msg roi xu li
    int server_fd, sckclient, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "socket failed\n";
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        std::cerr << "setsockopt failed\n";
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        std::cerr << "bind failed";
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0)
    {
        std::cerr << "listen failed\n";
        exit(EXIT_FAILURE);
    }

    char buff[1024];
    while (1)
    {
        if ((sckclient = accept(server_fd, (struct sockaddr *)&address,(socklen_t *)&addrlen)) < 0)
        {
            std::cerr << "accept failed\n";
            exit(EXIT_FAILURE);
        }
        memset(buff, 0, sizeof(buff));
        if (recv(sckclient, buff, sizeof(buff), 0) < 0)
        {
            std::cerr << "recv error\n";
        }
        else
        {
            cout<<buff<<endl;
           /* struct message msgSend;
	
      msgSend.msg_type = 1;
            (void)strcpy(msgSend.msg, buff);
            msgsnd(msg_id, &msgSend, sizeof(message), 0);
            cout<<msgSend.msg<<endl;*/
		string msg(buff);
		log_open();
		log(msg);
		log_close();
            close(sckclient);

        }
    }
}

