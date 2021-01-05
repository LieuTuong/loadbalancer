#include "log.h"

void log_init(string filename, string ip)
{
    logfd.open(filename);
    if (logfd.is_open())
    {
        logfd << "===================================" << endl;
        logfd << "====== Process " << ip << "'s LOGS FILE ======" << endl;
        logfd << "===================================\n\n\n";
    }
}

void log_close()
{
    logfd.close();
}

string log(string time, string ip, int port)
{
    string tmp;
    tmp.append("ip: ").append(ip).append(" | ");
    tmp.append("port: ").append(to_string(port)).append(" | ");
    tmp.append("time: ").append(time);
    logfd << tmp;
    return tmp;
}

void log(string msg)
{
    logfd << msg;
}

void log_terminal(string time, string ip, int port)
{
    cout << "ip: " << ip << " | "
         << "port: " << port << " | "
         << "time: " << time << endl;
}