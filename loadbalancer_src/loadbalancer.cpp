#include "loadbalancer.h"
#include <poll.h>
#include <vector>

uint BUF_SIZE = 20000;

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
                        //chon ws de xu ly
                        string msg(buffer);
                        string cookie_ws_ip = get_cookie_wsip(msg);
                        cout<<cookie_ws_ip<<endl;
                        shared_ptr<backend> myws = lb->choose_webserver(cookie_ws_ip);
                       
                        //myws = lb->select();
                        std::cout << "id: " << myws->get_id() << ", host: " << myws->get_host() << ", port: " << myws->get_port() << ", alive: " << myws->get_alive() << std::endl;

                        //ghi log va xu ly msg_queue
                        handle_log_active(it->fd, myws);

                        // proxy
                        proxy_handler(myws, it->fd, msg);
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

void handle_log_active(int client_sock, shared_ptr<backend> ws)
{
    // get client addr
    sockaddr_in sa = get_client_addr(client_sock);
    int port = htons(sa.sin_port);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sa.sin_addr), ip, INET_ADDRSTRLEN);

    // get current time
    string timenow = get_time();

    // write to log
    string msg_log = log(timenow, ip, port);
    log_terminal(timenow, ip, port);

    // add to msg queue
    struct message msgSend;
    msgSend.msg_type = 1;
    (void)strcpy(msgSend.msg, msg_log.c_str());
    msgsnd(msqid, &msgSend, sizeof(message), 0);
}

void handle_log_standby()
{
    // doc tu msg_queue
    while (1)
    {
        message msgRecv;
        if (msgrcv(msqid, &msgRecv, sizeof(message), 0, 0) < 0)
        {
            cerr << "msgrcv() error in handle_log_standby !" << endl;
        }

        else // ghi vao log
        {
            log(msgRecv.msg);
        }
    }
}

string get_cookie_wsip(string msg)
{
    string ip;
    if (msg.find("Cookie:") != string::npos)
    {
        int ip_start = msg.find("SERVERID=") + strlen("SERVERID=");
        int ip_end = msg.find_first_of("\r\n", ip_start);
        ip = msg.substr(ip_start, ip_end - ip_start);
    }
    return ip;
}

//========================== PROXY's AREA ===========================

void change_header(vector<char> &header, string ws_ip)
{
    string insert_str = "Set-cookie: SERVERID=";
    insert_str.append(ws_ip);
    insert_str.append("\r\n");

    int index;
    for (int i = 0; i < header.size(); i++)
    {
        if (header[i] == '\r' && header[i + 1] == '\n')
        {
            header.insert(header.begin() + i + 2, insert_str.begin(), insert_str.end());
            return;
        }
    }
}

void proxy_handler(const shared_ptr<backend> &ws, int client_sock, string client_request)
{
    // ket noi toi ws
    int ws_sockfd = Socket();
    Connect(ws_sockfd, ws->get_host(), ws->get_port());

    // gui request
    Send(ws_sockfd, client_request.c_str(), client_request.length());

    // nhan request
    int recv_each;
    char buff[BUF_SIZE];
    vector<char> response_vec;
    while (1)
    {
        recv_each = recv(ws_sockfd, buff, BUF_SIZE, 0);
        if (recv_each <= 0)
            break;

        // them vao vector
        for (int i = 0; i < recv_each; i++)
        {
            response_vec.push_back(buff[i]);
        }
        memset(buff, 0, BUF_SIZE);
    }

    // set lai header
    change_header(response_vec, ws->get_host());

    //cout << send(client_sock, &response_vec[0], response_vec.size(), 0) << endl;
    Send(client_sock, &response_vec[0], response_vec.size());

    close(ws_sockfd);
    close(client_sock);
}