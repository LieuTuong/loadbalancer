#include "socket.h"
#include "loadbalancer.h"
#include "log.h"
#include "config.h"
#include <sys/msg.h>
#include <sys/ipc.h>

using namespace std;

int msqid;

int main(int argc, char **argv)
{
    if (argc != 3 && argc != 4)
    {
        cout << "Not enough argument!!" << endl;
        return 0;
    }

    string ip = argv[1];
    int port = stoi(argv[2]);

    init_log(ip);
    init_msg_queue();

    vector<shared_ptr<backend>> backends;
    config_from_file(backends);

    shared_ptr<lb_base> lb(new lb_rr(backends));

    if (argc == 4)
    {
        lb->set_active(true);
    }

    if (lb->get_active())
    {
        start_server(ip, port, lb);
    }
    else
    {
        handle_log_standby();
    }
    

    return 0;
}