#include"socket.h"
#include"loadbalancer.h"

using namespace std;


int main(int argc, char** argv)
{
    if (argc!=3)
    {
        cout<<"Not enough argument!!"<<endl;
        return 0;
    }

    string ip = argv[1];
    int port=stoi(argv[2]);
    
    start_server(ip, port);
}