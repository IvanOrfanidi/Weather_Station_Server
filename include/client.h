#pragma once

#include <include/config.h>
#include <include/utils/safe_queue.h>
#include <tcp_client.h>
#include <thread>

class Client {
public:
    explicit Client(config::Net& config);

    void sendData(std::vector<char>& data);

private:
    static constexpr unsigned ATTEMPT_SEND_DATA = 10;

    void execute();

    tcp_udp_client::TCP_Client _client;
    threadsafe_queue<std::vector<char>> _data;
    size_t _sizeBuffer;
    unsigned _timeoutReset;
    std::thread _thread;
};
