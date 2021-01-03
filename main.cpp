#include "socket.h"
#include "loadbalancer.h"
#include <fstream>
#include <sstream>
using namespace std;

void config_from_file(vector<shared_ptr<backend>> backends)
{
    ifstream infile("webserver.txt");
    string line;

    while (getline(infile, line))
    {
        shared_ptr<backend> ws(new backend());
        string tmp;
        line.erase(0, string("process:").length());
        stringstream check(line);

        //get host
        getline(check, tmp, ':');
        ws->set_host(tmp);

        // get port
        getline(check, tmp, ':');
        ws->set_port(stoi(tmp));

        // get alive
        getline(check, tmp, ':');
        bool alive = stoi(tmp) == 1 ? true : false;
        ws->set_alive(alive);
        
        backends.push_back(ws);
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cout << "Not enough argument!!" << endl;
        return 0;
    }

    string ip = argv[1];
    int port = stoi(argv[2]);
}