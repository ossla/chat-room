#include "server.cpp"

int main() {
    WinsockLib::Instance();
    try {
        ChatServer server(std::cout);
        server.HandleConnections();

    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}