#ifndef __LOADBALANCER_H__
#define __LOADBALANCER_H__
#include "socket.h"
#include "log.h"
#include "config.h"
#include <string.h>


extern vector<pollfd> poll_sets;
extern uint BUF_SIZE;
extern int msqid;

class backend
{
private:
    uint id_ = 0;
    string host_;
    uint port_;
    bool alive_ = true;

public:
    backend() {}
    backend(const uint id, const string &host, const uint port, const bool alive) : id_(id), host_(host), port_(port), alive_(alive){};

    uint32_t get_id() { return id_; }
    void set_id(const uint id) { id_ = id; }

    uint32_t get_port() { return port_; }
    void set_port(const uint port) { port_ = port; }

    string get_host() { return host_; }
    void set_host(string host) { host_ = host; }

    bool get_alive() { return alive_; }
    void set_alive(bool alive) { alive_ = alive; }
};

class lb_base
{
protected:
    virtual uint32_t do_select()
    {
        return cur_idx;
    }

    uint32_t cur_idx = 0;
    vector<std::shared_ptr<backend>> backends_;
    bool active_ = false;

public:
    lb_base() {}
    lb_base(const vector<std::shared_ptr<backend>> backends) : backends_(backends) {}
    virtual ~lb_base() {}

    virtual shared_ptr<backend> select()
    {
        uint32_t idx = do_select();
        if (backends_.empty() || idx >= backends_.size())
        {
            return nullptr;
        }

        if (!backends_.at(idx)->get_alive())
            return select();
        else
            return backends_.at(idx);
    }
    void set_active(bool active) { active_ = active; }
    bool get_active() { return active_; }
};

class lb_rr : public lb_base
{
protected:
    uint32_t do_select()
    {
        if (cur_idx >= backends_.size())
        {
            cur_idx = 0;
        }
        return cur_idx++;
    }

public:
    lb_rr() {}
    lb_rr(const vector<std::shared_ptr<backend>> backends)
        : lb_base::lb_base(backends) {}
    ~lb_rr() {}
};

void start_server(string ip, int port, shared_ptr<lb_base> &lb);

void handle_log_active(int client_sock);

void handle_log_standby();

char* change_header(string header, string ws_ip);

void proxy_handler( const shared_ptr<backend>& ws,int client_sock, string client_request);

#endif