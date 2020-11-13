#include <cstdint>
#include <include/client.h>
#include <include/config.h>
#include <include/handler.h>
#include <include/server.h>
#include <iostream>
#include <project_version.h>
#include <string>

std::string parseCommand(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i) {
        const std::string arg = argv[i];

        // Запрос на версию ПО
        if (arg == "-v" || arg == "--version") {
            return PROJECT_VERSION;
        }
    }
    return "";
}

int main(int argc, char* argv[])
{
    // Парсер команд при старте
    const auto msgOut = parseCommand(argc, argv);
    if (msgOut.length()) {
        std::cout << msgOut << std::endl;
        return EXIT_SUCCESS;
    }

    // Таймаут на запуск при старте устройства
    std::this_thread::sleep_for(std::chrono::minutes(1));

    auto& config = Config::getInstance();
    Server server(config.getNetServerConfig());
    Client client(config.getNetClientConfig());

    Handler handler(server, client,
        config.getServerConfig(),
        config.getClientConfig(),
        config.getSizeBuffer());

    try {
        // Главный цикл выполнения
        handler.execute();
    } catch (const std::exception& e) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
