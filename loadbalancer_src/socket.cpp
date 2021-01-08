#include "socket.h"

string ERR_MSG = "Too many clients!!";
vector<pollfd> poll_sets;

int Socket()
{
    int lsockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lsockfd < 0)
    {
        cerr << "Socket() error!" << endl;
        exit(EXIT_FAILURE);
    }
    return lsockfd;
}

void Setsockopt(int lsockfd)
{
    int ret;
    if (setsockopt(lsockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &ret, sizeof(ret)) < 0)
    {
        cerr << "Setsockopt() error!" << endl;
        exit(EXIT_FAILURE);
    }
}

void Bind(int lsockfd, string ip, int port)
{
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr.s_addr);

    if (bind(lsockfd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cerr << "Bind() error!" << endl;
        exit(EXIT_FAILURE);
    }
}

void Listen(int lsockfd)
{
    if (listen(lsockfd, MAX_CLIENTS) < 0)
    {
        cerr << "Listen() error!" << endl;
        exit(EXIT_FAILURE);
    }
}

int Accept(int lsockfd)
{
    sockaddr_in client;
    socklen_t addr_size = sizeof(sockaddr_in);
    return accept(lsockfd, (sockaddr *)&client, &addr_size);
}

int Connect(int sockfd, string ip, int port)
{
    struct sockaddr_in srv_addr;
    srv_addr.sin_family=AF_INET;
    srv_addr.sin_port=htons(port);
    inet_pton(AF_INET, ip.c_str(),&srv_addr.sin_addr);
    return connect(sockfd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
}

int Send(int sockfd, const char *buff, int bufflen)
{
    int total_sent = 0;
    int senteach;
    while (total_sent < bufflen)
    {
        if ((senteach = send(sockfd, (void *)(buff + total_sent), bufflen - total_sent, 0)) < 0)
        {
            cerr << "Error in sending to server !" << endl;
            return senteach;
        }
        total_sent += senteach;
    }
    return total_sent;
}

sockaddr_in get_client_addr(int sock)
{
    sockaddr_in client;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    if (getpeername(sock, (struct sockaddr *)&client, &addr_size) != 0)
    {
        cerr << "get_client_addr error";
        exit(EXIT_FAILURE);
    }
    return client;
}

void server_overwhelmed(int client_sock)
{
    cout << ERR_MSG << endl;
    send(client_sock, ERR_MSG.c_str(), ERR_MSG.length(), 0);
    close(client_sock);
}

void print_start(string ip, int port)
{
    cout << "Server started success, listen at " << ip << ":" << port << endl;
}

void add_client(int client_sock)
{
    pollfd client_pollfd;
    client_pollfd.fd = client_sock;
    client_pollfd.events = POLLIN;
    poll_sets.push_back(client_pollfd);
}

void del_client(vector<pollfd>::iterator it)
{
    poll_sets.erase(it);
    close(it->fd);
}

