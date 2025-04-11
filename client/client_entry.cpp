// #include "client.cpp"
#include "client_ui.cpp"


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: <host> <port>" << std::endl;
        return 1;
    }

    WinsockLib::Instance();
    try {
        auto client = std::make_shared<ChatClient>(std::cout);
        client->MakeConnectionWith(argv[1], argv[2]);
        UI::RenderUI(client);

    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 2;
    }

    return 0;
}