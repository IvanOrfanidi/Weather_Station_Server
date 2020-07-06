#include <include/config.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unistd.h>

namespace error_message {
const std::string OPEN_CONFIG_FILE = "opening configuration file";
const std::string CONFIG_IN_FILE = "config in file";
const std::string OPEN_SERVER_FILE = "opening server configuration file";
const std::string OPEN_CLIENT_FILE = "opening client configuration file";
}

Config::Config()
{
    const std::string NAME_CONFIGURATION_FILE = "config.cfg";
    const std::string NAME_SERVER_CONFIGURATION_FILE = "server.cfg";
    const std::string NAME_CLIENT_CONFIGURATION_FILE = "client.cfg";

    std::string path;
    constexpr size_t PATH_SIZE = 1024;
    path.resize(PATH_SIZE);
#if defined(__unix__)
    size_t pos = readlink("/proc/self/exe", path.data(), PATH_SIZE);
    path.resize(pos);
    pos = path.rfind("/");
    path.resize(++pos);
#endif

#if defined(_WIN32)
    char currentWorkDir[PATH_SIZE];
    _getcwd(currentWorkDir, sizeof(currentWorkDir));
    path = std::string(currentWorkDir);
    path += "\\";
#endif

    std::string pathFile;
    pathFile = path + NAME_CONFIGURATION_FILE;
    openConfigFile(pathFile.data(), _netServer, _netClient);

    pathFile = path + NAME_SERVER_CONFIGURATION_FILE;
    openServerFile(pathFile.data(), _server);

    pathFile = path + NAME_CLIENT_CONFIGURATION_FILE;
    openClientFile(pathFile.data(), _client);
}

void Config::openConfigFile(const char* nameFile, config::Net& server, config::Net& client)
{
    std::ifstream file;
    file.open(nameFile, std::ifstream::in);
    if (!file.is_open()) {
        throw std::runtime_error(error_message::OPEN_CONFIG_FILE);
    }

    auto index = 0;
    const std::string DELIMITER = ";";
    std::string line;
    while (std::getline(file, line)) {
        std::cout << "<" << line << ">" << std::endl;

        size_t pos = 0;
        std::vector<std::string> token;
        while ((pos = line.find(DELIMITER)) != std::string::npos) {
            token.push_back(line.substr(0, pos));
            line.erase(0, pos + DELIMITER.length());
        }

        constexpr size_t NUMBER_CONFIG = 4;
        if (token.size() == NUMBER_CONFIG) {
            config::Net tempConfig;
            if (*token[0].begin() != '#') {
                tempConfig.addr = token[0];
            }
            if (*token[1].begin() != '#') {
                tempConfig.port = std::stoi(token[1]);
            }
            if (*token[2].begin() != '#') {
                tempConfig.sizeBuffer = std::stoi(token[2]);
            }
            if (*token[3].begin() != '#') {
                tempConfig.timeoutReset = std::stoi(token[3]);
            }

            if (tempConfig.addr.length() != 0 && tempConfig.port != 0 && tempConfig.sizeBuffer != 0 && tempConfig.timeoutReset != 0) {
                switch (index) {
                case 0:
                    server = tempConfig;
                    break;
                case 1:
                    client = tempConfig;
                    break;
                default:
                    file.close();
                    throw std::runtime_error(error_message::CONFIG_IN_FILE);
                    break;
                }
                index++;
                std::cout << std::endl;
            }
        }
    }

    file.close();
    constexpr auto VALID_INDEX = 2;
    if (index != VALID_INDEX) {
        throw std::runtime_error(error_message::CONFIG_IN_FILE);
    }
}

void Config::openServerFile(const char* nameFile, std::vector<config::Server>& config)
{
    std::ifstream file;
    file.open(nameFile, std::ifstream::in);
    if (!file.is_open()) {
        throw std::runtime_error(error_message::OPEN_SERVER_FILE);
    }

    const std::string DELIMITER = ";";
    std::string line;
    while (std::getline(file, line)) {
        std::cout << "<" << line << ">" << std::endl;

        size_t pos = 0;
        std::vector<std::string> token;
        while ((pos = line.find(DELIMITER)) != std::string::npos) {
            token.push_back(line.substr(0, pos));
            line.erase(0, pos + DELIMITER.length());
        }
        constexpr size_t NUMBER_SERVER_CONFIG = 3;
        if (token.size() == NUMBER_SERVER_CONFIG) {
            config::Server tempConfig;
            if (*token[0].begin() != '#') {
                tempConfig.key = token[0];
            }
            if (*token[1].begin() != '#') {
                tempConfig.numFields = std::stoi(token[1]);
            }
            if (*token[2].begin() != '#') {
                tempConfig.timeUpdate = std::stoi(token[2]);
            }

            if (!tempConfig.key.empty() && tempConfig.numFields > 0) {
                std::cout << "SERVER: Key = " << tempConfig.key << "\n"
                          << "SERVER: Number Fields = " << tempConfig.numFields << "\n"
                          << "SERVER: Time Update = " << tempConfig.timeUpdate << "\n"
                          << std::endl;

                config.push_back(tempConfig);
            }
            std::cout << std::endl;
        }
    }

    file.close();
}

void Config::openClientFile(const char* nameFile, std::vector<config::Client>& config)
{
    std::ifstream file;
    file.open(nameFile, std::ifstream::in);
    if (!file.is_open()) {
        throw std::runtime_error(error_message::OPEN_CLIENT_FILE);
    }

    constexpr std::string_view DELIMITER = ";";
    std::string line;
    while (std::getline(file, line)) {
        std::cout << "<" << line << ">" << std::endl;

        size_t pos = 0;
        std::vector<std::string> token;
        while ((pos = line.find(DELIMITER)) != std::string::npos) {
            token.push_back(line.substr(0, pos));
            line.erase(0, pos + DELIMITER.length());
        }
        constexpr size_t NUMBER_CLIENT_CONFIG = 4;
        if (token.size() == NUMBER_CLIENT_CONFIG) {
            config::Client tempConfig;
            if (*token[0].begin() != '#') {
                tempConfig.sourceKey = token[0];
            }
            if (*token[1].begin() != '#') {
                tempConfig.sourceChannel = std::stoi(token[1]);
            }
            if (*token[2].begin() != '#') {
                tempConfig.destinationKey = token[2];
            }
            if (*token[3].begin() != '#') {
                tempConfig.destinationChannel = std::stoi(token[3]);
            }
            if (!tempConfig.sourceKey.empty() && tempConfig.sourceChannel > 0 && !tempConfig.destinationKey.empty() && tempConfig.destinationChannel > 0) {
                std::cout << "CLIENT: Source Key = " << tempConfig.sourceKey << "\n"
                          << "CLIENT: Source Channel = " << tempConfig.sourceChannel << "\n"
                          << "CLIENT: Destination Key = " << tempConfig.destinationKey << "\n"
                          << "CLIENT: Destination Channel = " << tempConfig.destinationChannel
                          << std::endl;

                config.push_back(tempConfig);
            }
            std::cout << std::endl;
        }
    }

    file.close();
}

config::Net& Config::getNetServerConfig() noexcept
{
    return _netServer;
}

config::Net& Config::getNetClientConfig() noexcept
{
    return _netClient;
}

std::vector<config::Server>& Config::getServerConfig() noexcept
{
    return _server;
}

std::vector<config::Client>& Config::getClientConfig() noexcept
{
    return _client;
}

size_t Config::getSizeBuffer() const noexcept
{
    return (_netServer.sizeBuffer > _netClient.sizeBuffer) ? _netServer.sizeBuffer : _netClient.sizeBuffer;
}
