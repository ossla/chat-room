#include "client.hpp"

ChatClient::ChatClient(std::ostream& os) : log_os_(os) {}
ChatClient::ChatClient(ChatClient&& other) : log_os_(other.log_os_) 
                                , peer_socket_(other.peer_socket_) {
    other.peer_socket_ = -1;
}
ChatClient::~ChatClient() noexcept {
    Disconnect();
}

inline void ChatClient::MakeConnectionWith(const char* host, const char* port) {
    using namespace std::literals;
    log_os_ << "============== Connecting to " << host << ' ' << port << " ==============\n"
            << "Configuring remote address...\n";
    struct addrinfo raw_address;
    memset(&raw_address, 0, sizeof(raw_address));
    raw_address.ai_socktype = SOCK_STREAM;
    struct addrinfo* bind_address;
    if (getaddrinfo(host, port, &raw_address, &bind_address)) {
        throw SocketException("getaddrinfo() err. Code: " + STRING_SOCKET_ERRNO());
    }

    log_os_ << "Socket initialization...\n";
    peer_socket_ = socket(bind_address->ai_family
                        , bind_address->ai_socktype
                        , bind_address->ai_protocol);
    if (!ISVALIDSOCKET(peer_socket_)) {
        throw SocketException("socket() err. Code: " + STRING_SOCKET_ERRNO());
    }

    log_os_ << "Connecting to peer...\n"sv;
    if (connect(peer_socket_, bind_address->ai_addr
                , bind_address->ai_addrlen)) { // 
        throw SocketException("connect() err. Code: " + STRING_SOCKET_ERRNO());
    }
    freeaddrinfo(bind_address);
    log_os_ << "Connected to peer.\n";
}

void ChatClient::Disconnect() {
    std::lock_guard<std::mutex> lock(peer_mutex_);
    CLOSESOCKET(peer_socket_);
    peer_socket_ = -1;
}

void ChatClient::StartRecvMessages() {
    std::thread([this]() {
        RecvMessages();
    }).detach();
}

void ChatClient::RecvMessages() {
    char buffer[1024];

    while (1) {
        int bytes_received = recv(peer_socket_, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            std::cout << "Disconnected\n";

            Disconnect();
            throw SocketException("disconnection");
        }

        std::string msg(buffer, bytes_received);
        {
            std::lock_guard<std::mutex> lock(peer_mutex_);
            std::cout << "message from another person: " << msg << "\n";
        }

        if (on_message_received) {
            on_message_received(msg); // сообщаем UI
        }
    }
}

void ChatClient::SendMessage(const char msg[], size_t size) {
    std::lock_guard<std::mutex> lock(peer_mutex_);
    send(peer_socket_, msg, size, 0);
}
