
#include <include/client.h>
#include <cstdio>
#include <iostream>

Client::Client(config::Net& config)
    : _client(config.addr.data(), config.port)
    , _sizeBuffer(config.sizeBuffer)
    , _timeoutReset(config.timeoutReset)
{
    _thr = std::thread(&Client::execute, this);
}

void Client::sendData(std::vector<char>& data)
{
    _data.push(data);
}

void Client::execute()
{
    std::vector<char> data;
    data.reserve(_sizeBuffer);
    while (true) {
        _data.wait_and_pop(data);

        size_t countErr = 0;
        while (!data.empty()) {
            try {
                _client.connecting();
            } catch (const std::exception& e) {
                std::cout << "error: client connecting" << '\n';
                std::this_thread::sleep_for(std::chrono::milliseconds(_timeoutReset));
            }

            // Sendinng data and Receive answer
            try {
                _client.send(data);
                data.clear();
            } catch (const std::exception& e) {
                std::cout << "error: client send" << '\n';
                countErr++;
                std::this_thread::sleep_for(std::chrono::milliseconds(_timeoutReset));
            }

            if (countErr > ATTEMPT_SEND_DATA) {
                data.clear();
            }
        }
    }
}
