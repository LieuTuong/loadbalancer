#include "loadbalancer.h"

uint BUF_SIZE = 5000;

void start_server(string ip, int port, shared_ptr<lb_base> &lb)
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
                        server_overwhelmed(client_sock);
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
                        // chon server theo kieu rr
                        auto myws = lb->select();
                        std::cout << "id: " << myws->get_id() << ", host: " << myws->get_host() << ", port: " << myws->get_port() << ", alive: " << myws->get_alive() << std::endl;

                        // proxy
                        proxy_handler(myws, it->fd, string(buffer));

                        //ghi log va xu ly msg_queue
                        handle_log(it->fd);
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

void handle_log(int client_sock)
{
    // get client addr
    sockaddr_in sa = get_client_addr(client_sock);
    int port = htons(sa.sin_port);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sa.sin_addr), ip, INET_ADDRSTRLEN);

    // get current time
    string timenow = get_time();

    // write to log
    log(timenow, ip, port);
    log_terminal(timenow, ip, port);

    // add to msg queue
}

//========================== PROXY SIZE ===========================

char *change_header(string header, string ws_ip)
{
    string insert_str = "Set-cookie: SERVERID=";
    insert_str.append(ws_ip);
    insert_str.append("\r\n");
    int pos = header.find_first_of("\r\n");
    header.insert(pos + strlen("\r\n"), insert_str);
    char *res = new char[header.length()];
    memcpy(res, header.c_str(), header.length());
    return res;
}

void proxy_handler(const shared_ptr<backend> &ws, int client_sock, string client_request)
{
    // ket noi toi ws
    int ws_sockfd = Socket();
    Connect(ws_sockfd, ws->get_host(), ws->get_port());

    // gui request
    Send(ws_sockfd, client_request.c_str(), client_request.length());

    // nhan request && set lai header && gu ilai cho client
    int recv_each;
    char response[BUF_SIZE];
    while ((recv_each = recv(ws_sockfd, response, BUF_SIZE, 0)) > 0)
    {
        char *changed = change_header(string(response), ws->get_host());
        Send(client_sock, changed, strlen(changed));
        memset(response, 0, sizeof response);
        delete[] changed;
    }

    close(ws_sockfd);
}