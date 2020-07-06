#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include <include/utils/singleton.h>

namespace config {

struct Net {
    std::string addr;
    uint16_t port = 0;
    size_t sizeBuffer = 0;
    unsigned timeoutReset = 0;
};

struct Server {
    std::string key;
    int numFields = 0;
    unsigned timeUpdate = 0;
};

struct Client {
    std::string sourceKey;
    int sourceChannel = 0;
    std::string destinationKey;
    int destinationChannel = 0;
};
}

class Config : public Singleton<Config> {
public:
    Config();

    friend class Singleton<Config>;

    config::Net& getNetServerConfig() noexcept;

    config::Net& getNetClientConfig() noexcept;

    std::vector<config::Server>& getServerConfig() noexcept;

    std::vector<config::Client>& getClientConfig() noexcept;

    size_t getSizeBuffer() const noexcept;

private:
    void openConfigFile(const char* nameFile, config::Net& server, config::Net& client);

    void openServerFile(const char* nameFile, std::vector<config::Server>& config);

    void openClientFile(const char* nameFile, std::vector<config::Client>& config);

    std::vector<config::Server> _server;
    std::vector<config::Client> _client;
    config::Net _netServer;
    config::Net _netClient;
};