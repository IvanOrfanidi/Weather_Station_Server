#include <include/server.h>
#include <iostream>

Server::Server(config::Net& config)
    : _server(config.addr.data(), config.port)
    , _sizeBuffer(config.sizeBuffer)
    , _timeoutReset(config.timeoutReset)
{
    try {
        _server.start();
    } catch (const std::exception& e) {
        std::cout << "error: server init" << '\n';
        throw e;
    }

    _thr = std::thread(&Server::execute, this);
}

void Server::receiveData(std::vector<char>& data)
{
    _data.wait_and_pop(data);
}

bool Server::empty() const
{
    return _data.empty();
}

void Server::restart()
{
    try {
        _server.stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(_timeoutReset));
        _server.start();
    } catch (const std::exception& e) {
        std::cout << "error: server restart" << '\n';
    }
}

void Server::execute()
{
    std::vector<char> data;
    data.reserve(_sizeBuffer);
    while (true) {
        try {
            _server.accept();
        } catch (const std::exception& e) {
            std::cout << "error: server accept" << '\n';
            restart();
            continue;
        }

        data.resize(_sizeBuffer);
        try {
            _server.receive(data, _sizeBuffer);
        } catch (const std::exception& e) {
            std::cout << "error: server receive" << '\n';
        }

        if (!data.empty()) {
            // Copy data
            _data.push(data);
            data.clear();
        }
    }
}