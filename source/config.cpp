#include <include/config.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

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
    // Получаем директорию где находится бинарник
    const std::string currentWorkDir = getCurrentWorkDir();

    // Получаем пути к файлам конфигураций
    const std::string pathToConfigFile = currentWorkDir + CONFIGURATION_DIRECTORY.data() + NAME_CONFIGURATION_FILE.data();
    const std::string pathToServerFile = currentWorkDir + CONFIGURATION_DIRECTORY.data() + NAME_SERVER_CONFIGURATION_FILE.data();
    const std::string pathToClientFile = currentWorkDir + CONFIGURATION_DIRECTORY.data() + NAME_CLIENT_CONFIGURATION_FILE.data();

    openConfigFile(pathToConfigFile.data(), _netServer, _netClient);
    openServerFile(pathToServerFile.data(), _server);
    openClientFile(pathToClientFile.data(), _client);
}

void Config::openConfigFile(const char* nameFile, config::Net& server, config::Net& client)
{
    std::ifstream file;
    file.open(nameFile, std::ifstream::in);
    if (!file.is_open()) {
        throw std::runtime_error(error_message::OPEN_CONFIG_FILE);
    }

    size_t index = 0;
    std::string line;
    while (std::getline(file, line)) {
        std::cout << "<" << line << ">" << std::endl;

        std::vector<std::string> token;
        boost::split(token, line, boost::is_any_of(TOKEN_FINDER));

        if (token.size() == NUMBER_CONFIG + 1) {
            config::Net configuration;
            if (*token[0].begin() != '#') {
                configuration.addr = token[0];
            }
            if (*token[1].begin() != '#') {
                configuration.port = std::stoi(token[1]);
            }
            if (*token[2].begin() != '#') {
                configuration.sizeBuffer = std::stoi(token[2]);
            }
            if (*token[3].begin() != '#') {
                configuration.timeoutReset = std::stoi(token[3]);
            }

            if (configuration.addr.length() != 0 && configuration.port != 0 && configuration.sizeBuffer != 0 && configuration.timeoutReset != 0) {
                switch (index) {
                case ConfigIndex::SERVER:
                    server = configuration;
                    break;
                case ConfigIndex::CLIENT:
                    client = configuration;
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
    if (index != ConfigIndex::END) {
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

    std::string line;
    while (std::getline(file, line)) {
        std::cout << "<" << line << ">" << std::endl;

        std::vector<std::string> token;
        boost::split(token, line, boost::is_any_of(TOKEN_FINDER));

        if (token.size() == NUMBER_SERVER_CONFIG + 1) {
            config::Server configuration;
            if (*token[0].begin() != '#') {
                configuration.key = token[0];
            }
            if (*token[1].begin() != '#') {
                configuration.numFields = std::stoi(token[1]);
            }
            if (*token[2].begin() != '#') {
                configuration.timeUpdate = std::stoi(token[2]);
            }

            if (!configuration.key.empty() && configuration.numFields > 0) {
                std::cout << "SERVER: Key = " << configuration.key << "\n"
                          << "SERVER: Number Fields = " << configuration.numFields << "\n"
                          << "SERVER: Time Update = " << configuration.timeUpdate << "\n"
                          << std::endl;

                config.push_back(configuration);
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

    std::string line;
    while (std::getline(file, line)) {
        std::cout << "<" << line << ">" << std::endl;

        std::vector<std::string> token;
        boost::split(token, line, boost::is_any_of(TOKEN_FINDER));

        if (token.size() == NUMBER_CLIENT_CONFIG + 1) {
            config::Client configuration;
            if (*token[0].begin() != '#') {
                configuration.sourceKey = token[0];
            }
            if (*token[1].begin() != '#') {
                configuration.sourceChannel = std::stoi(token[1]);
            }
            if (*token[2].begin() != '#') {
                configuration.destinationKey = token[2];
            }
            if (*token[3].begin() != '#') {
                configuration.destinationChannel = std::stoi(token[3]);
            }
            if (!configuration.sourceKey.empty() && configuration.sourceChannel > 0 && !configuration.destinationKey.empty() && configuration.destinationChannel > 0) {
                std::cout << "CLIENT: Source Key = " << configuration.sourceKey << "\n"
                          << "CLIENT: Source Channel = " << configuration.sourceChannel << "\n"
                          << "CLIENT: Destination Key = " << configuration.destinationKey << "\n"
                          << "CLIENT: Destination Channel = " << configuration.destinationChannel
                          << std::endl;

                config.push_back(configuration);
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
    // Вычисляем размер общего буфера
    const size_t maxBufferSize = (_netServer.sizeBuffer > _netClient.sizeBuffer) ? _netServer.sizeBuffer : _netClient.sizeBuffer;
    return maxBufferSize;
}

std::string Config::getCurrentWorkDir() const
{
    std::string path;
    static constexpr size_t PATH_SIZE = 1024;
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
    return path;
}
