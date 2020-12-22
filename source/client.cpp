
#include <cstdio>
#include <include/client.h>
#include <iostream>

Client::Client(config::Net& config)
    : _client(config.addr.data(), config.port)
    , _sizeBuffer(config.sizeBuffer)
    , _timeoutReset(config.timeoutReset)
{
    _thread = std::thread(&Client::execute, this);
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
            } catch (...) {
                std::cerr << "error: client connecting" << '\n';
                std::this_thread::sleep_for(std::chrono::milliseconds(_timeoutReset));
            }

            // Отправляем данные и принимаем подтверждение
            try {
                _client.send(data);
                data.clear();
            } catch (...) {
                std::cerr << "error: client send" << '\n';
                countErr++;
                std::this_thread::sleep_for(std::chrono::milliseconds(_timeoutReset));
            }

            if (countErr > ATTEMPT_SEND_DATA) {
                data.clear();
            }
        }
    }
}
