#include "config.h"

string config_file = "webservers.txt";


void init_msg_queue()
{
    key_t my_key=123;
    msqid = msgget(my_key, 0666 | IPC_CREAT);
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
        
        // get name
        
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

char *get_time()
{
    auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
    return ctime(&timenow);
}