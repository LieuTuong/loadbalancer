#include "header.h"

void send_mess(std::string ip, int port, std::string msg)
{
    int sock, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "\n Socket creation error \n";
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "\nInvalid address/ Address not supported \n";
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "\nConnection Failed \n";
        return;
    }

    send(sock, msg.c_str(), msg.length(), 0);
    close(sock);
}
