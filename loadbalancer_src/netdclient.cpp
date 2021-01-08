#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <csignal>
using namespace std;

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

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "\nConnection Failed \n";
        return;
    }

    send(sock, msg.c_str(), msg.length(), 0);
    close(sock);
}

int main()
{
    key_t my_key;
    my_key = ftok("netdclient", 65);
    msg_id = msgget(my_key, 0666 | IPC_CREAT);

    signal(SIGINT, sigStop_handler);

    while (1)
    {
        message msgRecv;
        if (msgrcv(msg_id, &msgRecv, sizeof(message), 1, 0) < 0)
        {
            cerr << "msgrcv error" << endl;
        }
        cout << msgRecv.msg << endl;
        send_mess(string("127.0.0.1"), 12345, string(msgRecv.msg));
    }
}