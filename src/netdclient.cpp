#include "header.h"
#include <csignal>
using namespace std;

int msg_id;

void sigStop_handler(int sig)
{
    msgctl(msg_id, IPC_RMID, NULL);
    exit(0);
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
        cout<<msgRecv.msg<<endl;
        send_mess(string("127.0.0.1"),12345,string(msgRecv.msg));
    }
}