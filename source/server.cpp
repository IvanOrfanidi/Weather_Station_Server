#include <include/server.h>
#include <iostream>

Server::Server(config::Net& config)
    : _server(config.addr.data(), config.port)
    , _sizeBuffer(config.sizeBuffer)
    , _timeoutReset(config.timeoutReset)
{
    try {
        _server.start();
    } catch (...) {
        std::cerr << "error: server init" << '\n';
        throw std::current_exception();
    }

    _thread = std::thread(&Server::execute, this);
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
    } catch (...) {
        std::cerr << "error: server restart" << '\n';
    }
}

void Server::execute()
{
    std::vector<char> data;
    data.reserve(_sizeBuffer);
    while (true) {
        try {
            _server.acceptSocket();
        } catch (...) {
            std::cerr << "error: server accept" << '\n';
            restart();
            continue;
        }

        data.resize(_sizeBuffer);
        try {
            _server.receiveData(data, _sizeBuffer);
        } catch (...) {
            std::cerr << "error: server receive" << '\n';
        }

        if (!data.empty()) {
            // Копируем данные
            _data.push(data);
            data.clear();
        }
    }
}