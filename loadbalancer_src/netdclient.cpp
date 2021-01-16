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
using namespace std;

string netdsrv_ip("172.18.0.3");
int netdsrv_port = 11111;

struct message
{
    long msg_type;
    char msg[100];
};

using namespace std;

int msg_id;

void sigStop_handler(int sig)
{
    msgctl(msg_id, IPC_RMID, NULL);
    exit(0);
}

void send_mess(std::string ip, int port, std::string msg)
{
    int sock, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "\n Socket creation error \n";
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "\nInvalid address/ Address not supported \n";
        return;
    }
    while (1)
    {
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            sleep(5);
        }
        else
        {
            break;
        }
    }

    send(sock, msg.c_str(), msg.length(), 0);
    close(sock);
}


int main()
{
    key_t my_key = 123;
    msg_id = msgget(my_key, 0666 | IPC_CREAT);

    signal(SIGINT, sigStop_handler);

    while (1)
    {
        message msgRecv;
        if (msgrcv(msg_id, &msgRecv, sizeof(message), 1, 0) < 0)
        {
            cerr << "msgrcv error" << endl;
        }
        send_mess(netdsrv_ip, netdsrv_port, string(msgRecv.msg));
        cout << msgRecv.msg << endl;
    }
}
