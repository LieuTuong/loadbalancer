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

void log(string time, string ip, int port)
{
    logfd << "time: " << time << " || "
          << "ip: " << ip << " || "
          << "port: " << port << endl;
}

void log_terminal(uint proposer, string recv_pid, bool ok)
{
}