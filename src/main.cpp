#include "socket.h"
#include "loadbalancer.h"
#include "log.h"
#include "config.h"

using namespace std;


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