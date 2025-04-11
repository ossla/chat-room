#include "server.hpp"

ChatServer::ChatServer(std::ostream& log_ostream) : log_os_(log_ostream) {
    InitializeServerSocket();
}
ChatServer::~ChatServer() noexcept {
    CLOSESOCKET(listen_socket_);
}

void ChatServer::InitializeServerSocket() {
    using namespace std::literals;
    log_os_ << "============== Configuring server socket ==============\n";

    struct addrinfo raw_address;
    std::memset(&raw_address, 0, sizeof(raw_address));
    raw_address.ai_family = AF_INET;
    raw_address.ai_socktype = SOCK_STREAM;
    raw_address.ai_flags = AI_PASSIVE;
    log_os_ << "Configuring local address...\n"sv;
    struct addrinfo* bind_address;
    if (getaddrinfo(0, "8080", &raw_address, &bind_address)) {
        throw SocketException("getaddrinfo() err. Code: " + STRING_SOCKET_ERRNO());
    }

    log_os_ << "Configuring listening socket...\n"sv; 
    listen_socket_ = socket(bind_address->ai_family
                            , bind_address->ai_socktype
                            , bind_address->ai_protocol);
    if (!ISVALIDSOCKET(listen_socket_)) {
        throw SocketException("socket() err." + STRING_SOCKET_ERRNO());
    }

    log_os_ << "Binding socket to local address...\n"sv;
    if (bind(listen_socket_, bind_address->ai_addr, bind_address->ai_addrlen)) {
        throw SocketException("bind() err. Code: " + STRING_SOCKET_ERRNO());
    }
    freeaddrinfo(bind_address);
}

void ChatServer::HandleConnections() {
    if (listen_socket_ == -1) {
        throw SocketException("Socket did not connected!");
    }
    log_os_ << "============== Handling connections ==============\n";
    log_os_ << "Listening...\n";
    if (listen(listen_socket_, 10) < 0) {
        throw SocketException("listen() err. Code: " + STRING_SOCKET_ERRNO());
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(listen_socket_, &master);
    int max_socket = listen_socket_;

    connections_.reserve(1024);
    while(1) {
        fd_set reads;
        reads = master;
        if (select(max_socket + 1, &reads, 0, 0, 0) < 0) {
            throw SocketException("select() err. Code: " + STRING_SOCKET_ERRNO());
        }

        // newbie
        if (FD_ISSET(listen_socket_, &reads)) {
            AcceptNewConnection(master, max_socket);
            log_os_ << "New connection accepted: " << addresses_.back() << "\n";
            continue;
        }

        // Handle client activity
        for (const auto& [current_socket, address_name_ptr]
                             : connections_) {
            if (FD_ISSET(current_socket, &reads)) {
                ProcessClientData(master, current_socket);
                break;
            }
        }
    }
}

void ChatServer::AcceptNewConnection(fd_set& master
                                            , int& max_socket) {

    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    SOCKET socket_client = accept(listen_socket_
                            , (struct sockaddr*) &client_address
                            , &client_len);
    if (!ISVALIDSOCKET(socket_client)) {
        throw SocketException("accept() err. Code: " + STRING_SOCKET_ERRNO());
    }

    FD_SET(socket_client, &master);
    if (socket_client > max_socket)
        max_socket = socket_client;

    // Get name of new client's address and put it to the storage.
    // SOCKET - address
    char address_buffer[100];
    getnameinfo((struct sockaddr*) &client_address
                , client_len, address_buffer
                , sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
    std::string address(address_buffer
                    , strnlen(address_buffer, sizeof(address_buffer)));

    // add to storage
    addresses_.push_back(std::move(address_buffer));
    std::unique_ptr<std::string> 
            last_el_ptr = std::make_unique<std::string>(addresses_.back());
    connections_[socket_client] = std::move(last_el_ptr);
}

void ChatServer::ProcessClientData(fd_set& master
                                   , SOCKET current_socket) {
    bool is_out = false;
    char read[1024];
    int bytes_received = recv(current_socket, read, 1024, 0);
    if (bytes_received < 1) {
        FD_CLR(current_socket, &master);
        CLOSESOCKET(current_socket);
        is_out = true;
    }

    std::string response = is_out ?
                            "[" + *connections_[current_socket] + "] left the chat\n"
                            :
                            "[" + *connections_[current_socket] + "]: " + std::string(read, bytes_received) + '\n';

    std::cout << response << "\n";
    for (const auto& [socket, address] : connections_) {
        if (socket == listen_socket_ || socket == current_socket)
            continue;
        else
            send(socket, response.c_str(), response.size(), 0);
    }
}