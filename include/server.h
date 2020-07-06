#pragma once

#include <include/config.h>
#include <include/utils/safe_queue.h>
#include <tcp_server.h>
#include <thread>

class Server {
public:
    explicit Server(config::Net& config);

    void receiveData(std::vector<char>& data);

    bool empty() const;

private:
    void execute();

    void restart();

    tcp_udp_server::TCP_Server _server;
    threadsafe_queue<std::vector<char>> _data;
    size_t _sizeBuffer;
    unsigned _timeoutReset;
    std::thread _thr;
};
