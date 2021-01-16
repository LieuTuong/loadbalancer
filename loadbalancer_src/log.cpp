#include "log.h"

void log_init()
{
    string fname(LOG_DIR);
    fname.append("log.txt");
    logfd.open(fname, fstream::app);
    if (logfd.is_open())
    {
        logfd << "===================================" << endl;
        logfd << " Server's LOGS FILE " << endl;
        logfd << "===================================\n\n\n";
    }
    logfd.close();
}

void log_close()
{
    logfd.close();
}

void log_open()
{
    string fname(LOG_DIR);
    fname.append("log.txt");
    logfd.open(fname, fstream::app);
}

string log(string time, string ip, int port, string ws_ip)
{
    log_open();
    string tmp;
    tmp.append("ip: ").append(ip).append(" | ");
    tmp.append("port: ").append(to_string(port)).append(" | ");
tmp.append("ws_ip: ").append(ws_ip).append(" | ");
    tmp.append("time: ").append(time);
    logfd << tmp;
    log_close();
    return tmp;
}

void log(string msg)
{
    logfd << msg;
}

void log_terminal(string time, string ip, int port)
{
    cout << CYAN << "ip: " << ip << " | "
         << "port: " << port << " | "
         << "time: " << time << " | ";
    //ws->print_ws_info();
    cout << RESET << endl;
}
