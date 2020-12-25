#pragma once

#include <cstdio>
#include <include/client.h>
#include <include/config.h>
#include <include/server.h>
#include <include/utils/safe_queue.h>
#include <thread>

class Handler {
public:
    struct Data {
        struct Value {
            std::string value;
            bool ready = false;
        };
        static constexpr size_t MAX_NUMBER_OF_FIELDS = 8;
        std::array<Value, MAX_NUMBER_OF_FIELDS> field;
    };

    Handler(Server& server,
        Client& client,
        std::vector<config::Server>& serverConfig,
        std::vector<config::Client>& clientConfig,
        size_t sizeBuffer);

    void execute();

    /**
     * @brief Парсер данных
     * 
     * @param dest выходной буфер данных предназначенных для серверов
     * @param src входной буфер данных идущих от клиента
     * @param serverConfig конфигурация сервера
     * @param clientConfig конфигурация клиента
     */
    void parserData(std::vector<Data>& dest,
        const std::vector<char>& src,
        const std::vector<config::Server>& serverConfig,
        const std::vector<config::Client>& clientConfig);

    /**
     * @brief Проверка готовности данных для отправки на сервер
     * 
     * @param dest выходной буфер который готов для отправки
     * @param src входные данные от клиентов серверу
     * @param serverConfig конфигурация сервера определяющая готовность к отправке
     * @return true данные готовы к отправке
     * @return false данные не готовы к отправке
     */
    bool checkData(std::vector<char>& dest,
        std::vector<Data>& src,
        const std::vector<config::Server>& serverConfig);

private:
    void outInfo(const std::vector<char>& data) const;

    void getValues(std::vector<std::string>& value,
        const std::vector<std::string>& token);

    Server& _server; //< Экзепляр сервера
    Client& _client; //< Экзепляр клиента

    std::vector<config::Server> _serverConfig; //< Конфигурация серверов, не более 4
    std::vector<config::Client> _clientConfig; //< Конфигурация клиентов, кол-во клиентов не ограничено
    std::vector<Data> _data; //< Преобразованные данные, кол-во элементов равно кол-ву конфигураций серверов

    size_t _sizeBuffer;
};