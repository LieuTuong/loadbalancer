#include "socket.h"
#include "loadbalancer.h"
#include "log.h"
#include <fstream>
#include <sstream>
using namespace std;
string config_file = "webservers.txt";


void init_log(string ip)
{
    //open file logs
    string fname = "../logs/";
    fname.append(ip);
    fname.append("_log.txt");
    log_init(fname, ip);
}

void config_from_file(vector<shared_ptr<backend>> &backends)
{
    ifstream infile(config_file);
    string line;

    while (getline(infile, line))
    {
        shared_ptr<backend> ws(new backend());
        string tmp;
        stringstream check(line);

        //get host
        getline(check, tmp, ':');
        ws->set_host(tmp);

        // get port
        getline(check, tmp, ':');
        ws->set_port(stoi(tmp));

        // get id
        getline(check, tmp, ':');
        ws->set_id(stoi(tmp));

        // get alive
        getline(check, tmp, ':');
        bool alive = stoi(tmp) == 1 ? true : false;
        ws->set_alive(alive);

        backends.push_back(ws);
    }
}

int main(int argc, char **argv)
{
    if (argc != 3 && argc != 4)
    {
        cout << "Not enough argument!!" << endl;
        return 0;
    }

    string ip = argv[1];
    int port = stoi(argv[2]);

    vector<shared_ptr<backend>> backends;
    config_from_file(backends);

    shared_ptr<lb_base> lb(new lb_rr(backends));

    if (argc == 4)
    {
        lb->set_active(true);
    }

    start_server(ip, port, lb);
   
    return 0;
}