
#include <cassert>
#include <chrono>
#include <cstring>
#include <fstream>
#include <include/handler.h>
#include <iomanip>
#include <iostream>


Handler::Handler(Server& server,
    Client& client,
    std::vector<config::Server>& serverConfig,
    std::vector<config::Client>& clientConfig,
    size_t sizeBuffer)
    : _server(server)
    , _client(client)
    , _serverConfig(serverConfig)
    , _clientConfig(clientConfig)
    , _sizeBuffer(sizeBuffer)
{
    // Определение кол-ва экземпляров данных равных кол-ву серверов
    _data.resize(serverConfig.size());
}

void Handler::execute()
{
    std::vector<char> data;
    data.reserve(_sizeBuffer);
    while (true) {
        if (!_server.empty()) {
            _server.receiveData(data);
            std::cout << "input data:" << std::endl;
            outInfo(data);

            if (!data.empty()) {
                parserData(_data, data, _serverConfig, _clientConfig);
            }

            if (checkData(data, _data, _serverConfig)) {
                std::cout << "output data:" << std::endl;
                _client.sendData(data);
                outInfo(data);
            }

            data.clear();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

/**
 * @brief Парсер данных
 * 
 * @param dest выходной буфер данных предназначенных для серверов
 * @param src входной буфер данных идущих от клиента
 * @param serverConfig конфигурация сервера
 * @param clientConfig конфигурация клиента
 */
void Handler::parserData(std::vector<Data>& dest,
    const std::vector<char>& src,
    const std::vector<config::Server>& serverConfig,
    const std::vector<config::Client>& clientConfig)
{
    std::string stringForParser(src.begin(), src.end());

    std::string key;
    size_t pos = 0;
    constexpr std::string_view START_OF_MESSAGE = "GET /update?key=";
    if ((pos = stringForParser.find(START_OF_MESSAGE.data())) != std::string::npos) {
        constexpr size_t SIZE_KEY_STRING = 16;
        key = stringForParser.substr(pos + START_OF_MESSAGE.length(), pos + SIZE_KEY_STRING);
    }

    constexpr std::string_view DELIMITER = "&";
    stringForParser.erase(0, key.length() + START_OF_MESSAGE.length() + DELIMITER.length());
    std::string field;
    std::vector<std::string> token;
    while ((pos = stringForParser.find(DELIMITER)) != std::string::npos) {
        field = stringForParser.substr(0, pos);
        token.push_back(field);
        stringForParser.erase(0, pos + DELIMITER.length());
    }
    if ((pos = stringForParser.find("\r\n")) != std::string::npos) {
        field = stringForParser.substr(0, pos);
        token.push_back(field);
    }

    // Выделяем чистые данные без имен полей
    std::vector<std::string> value;
    getValues(value, token);

    // Передаем данные серверам
    for (const auto& client : clientConfig) {
        // Определяем что клиент есть в списке обработки и есть этого значение поля
        if (client.sourceKey == key && value[client.sourceChannel - 1].length()) {
            // Находим сервер для которого предназначины эти данные
            assert(serverConfig.size() == dest.size());
            for (size_t i = 0; i < dest.size(); ++i) {
                if (client.destinationKey == serverConfig[i].key) {
                    dest[i].field[client.destinationChannel - 1].value = value[client.sourceChannel - 1];
                    dest[i].field[client.destinationChannel - 1].ready = true;
                }
            }
        }
    }
}

/**
 * @brief Проверка готовности данных для отправки на сервер
 * 
 * @param dest выходной буфер который готов для отправки
 * @param src входные данные от клиентов серверу
 * @param serverConfig конфигурация сервера определяющая готовность к отправке
 * @return true данные готовы к отправке
 * @return false данные не готовы к отправке
 */
bool Handler::checkData(std::vector<char>& dest,
    std::vector<Data>& src,
    const std::vector<config::Server>& serverConfig)
{
    assert(serverConfig.size() == src.size());
    for (size_t i = 0; i < src.size(); ++i) {
        bool ready = true;
        for (int n = 0; n < serverConfig[i].numFields; ++n) {
            if (!src[i].field[n].ready) {
                ready = false;
                break;
            }
        }

        if (ready) {
            // Формируем выходное сообщение
            // За один раз можем отправить данные только одному каналу
            std::string data = "GET /update?key="; //< Начало сообщения

            // Добавляем токен канала
            data += serverConfig[i].key;

            // Добавляем поля данных
            for (int n = 0; n < serverConfig[i].numFields; ++n) {
                data += "&field" + std::to_string(n + 1) + "=" + src[i].field[n].value;

                // Чистим поля чтобы не отправлял эти данные повторно
                src[i].field[n].value = "";
                src[i].field[n].ready = false;
            }
            // Завершение сообщения CR LF
            data += "\r\n";

            // Перекладываем данные в выходной вектор
            dest.clear();
            std::copy(data.begin(), data.end(), std::back_inserter(dest));
            return true;
        }
    }

    return false;
}

void Handler::outInfo(const std::vector<char>& data) const
{
    const time_t now = time(0);
    const tm* date = localtime(&now);

    std::cout << std::showbase << std::internal << std::setfill('0');
    std::cout << std::setw(2) << date->tm_mday << '/'
              << std::setw(2) << date->tm_mon + 1 << '/'
              << std::setw(2) << date->tm_year + 1900 << ' '
              << std::setw(2) << date->tm_hour << ':'
              << std::setw(2) << date->tm_min << ':'
              << std::setw(2) << date->tm_sec << ">\n";

    for (const auto& i : data) {
        std::cout << i;
    }
    std::cout << '\n';
}

void Handler::getValues(std::vector<std::string>& value,
    const std::vector<std::string>& token)
{
    value.reserve(Data::MAX_NUM_FIELD);
    value.resize(Data::MAX_NUM_FIELD);
    const std::vector<std::string> tokenFields{
        "field1=",
        "field2=",
        "field3=",
        "field4=",
        "field5=",
        "field6=",
        "field7=",
        "field8=",
    };

    for (const auto& i : token) {
        size_t numData = 0;
        for (const auto& n : tokenFields) {
            if (i.find(n) != std::string::npos) {
                value[numData] = i.substr(n.length(), i.length());
                break;
            }
            numData++;
        }
    }
}