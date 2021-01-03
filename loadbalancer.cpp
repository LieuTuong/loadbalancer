#include "loadbalancer.h"

uint BUF_SIZE = 1024;



void lb_base::start_server(string ip, int port)
{
    int lsockfd = Socket();
    Setsockopt(lsockfd);
    Bind(lsockfd, ip, port);
    Listen(lsockfd);
    print_start(ip, port);

    pollfd listen_pollfd;
    listen_pollfd.fd = lsockfd;
    listen_pollfd.events = POLLIN;
    poll_sets.push_back(listen_pollfd);

    while (1)
    {
        int ret = poll((pollfd *)&poll_sets[0], (unsigned int)poll_sets.size(), -1);
        if (ret <= 0)
        {
            cerr << "poll error" << endl;
            exit(EXIT_FAILURE);
        }

        vector<pollfd>::iterator it;
        vector<pollfd>::iterator end = poll_sets.end();
        for (it = poll_sets.begin(); it != end; ++it)
        {
            if (it->revents & POLLIN)
            {
                // new client
                if (it->fd == lsockfd)
                {
                    int client_sock = Accept(lsockfd);
                    if (client_sock == -1)
                    {
                        cerr << "accept() failed" << endl;
                        continue;
                    }

                    if (poll_sets.size() - 1 < MAX_CLIENTS)
                    {
                        cout << "add client" << endl;
                        add_client(client_sock);
                    }
                    else
                    {
                        server_overwheled(client_sock);
                    }
                }

                //client handle
                else
                {
                    char buffer[BUF_SIZE];
                    memset(buffer, 0, BUF_SIZE);
                    int num_read = recv(it->fd, buffer, BUF_SIZE, 0);
                    if (num_read <= 0)
                    {
                        del_client(it);
                    }
                    else
                    {
                        cout << buffer << endl;
                        //send(it->fd, buffer, strlen(buffer), 0);
                    }
                }
            }

            else if (it->revents & POLLERR)
            {
                if (it->fd == lsockfd)
                {
                    cerr << "listen socket error!!" << endl;
                    exit(EXIT_FAILURE);
                }

                else
                {
                    del_client(it);
                }
            }
        }
    }
}