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
    logfd << "ip: " << ip << "|" \
          << "port: " << port << "|" \
          << "time: " << time << endl;
}

void log_terminal(string time, string ip, int port)
{
    cout << "ip: " << ip << " | " \
         << "port: " << port << " | " \
         << "time: " << time << endl;
}