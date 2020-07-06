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
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--version") {
            return PROJECT_VERSION;
        }
    }
    return "";
}

int main(int argc, char* argv[])
{
    const auto msgOut = parseCommand(argc, argv);
    if (msgOut.length()) {
        std::cout << msgOut << std::endl;
        return EXIT_SUCCESS;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(60 * 1000));

    auto& config = Config::getInstance();
    Server server(config.getNetServerConfig());
    Client client(config.getNetClientConfig());

    Handler handler(server, client,
        config.getServerConfig(),
        config.getClientConfig(),
        config.getSizeBuffer());

    try {
        handler.execute();
    } catch (const std::exception& e) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
