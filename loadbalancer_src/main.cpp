#include "socket.h"
#include "loadbalancer.h"
#include "log.h"
#include "config.h"
using namespace std;

int msqid;
string lb_ip("172.18.0.200");
int lb_port = 8888;

int main(int argc, char **argv)
{
    /*if (argc != 3 && argc != 4)
    {
        cout << "Not enough argument!!" << endl;
        return 0;
    }*/

    /*lb_ip = argv[1];
    lb_port = stoi(argv[2]);*/

    log_init();
    init_msg_queue();

    vector<shared_ptr<backend>> backends;
    config_from_file(backends);

    shared_ptr<lb_base> lb(new lb_rr(backends));

    lb->set_active(true);

    if (lb->get_active())
    {
        start_server(lb_ip, lb_port, lb);
    }
    else
    {
        handle_log_standby();
    }

    return 0;
}