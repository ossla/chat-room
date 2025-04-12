
#pragma once
#include "../chat-room.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <deque>
#include <memory>
#include <charconv>



class ChatServer {
private:
    inline static const char PORT[] = "8080";
    std::ostream& log_os_;

    SOCKET listen_socket_ = -1;
    std::deque<std::string>             addresses_;
    std::unordered_map<SOCKET
        , std::unique_ptr<std::string>> connections_; /* stores SOCKET and ptr to the string with its address */

public:
    ChatServer() = delete;
    ChatServer(ChatServer&&) = delete;
    ChatServer(const ChatServer&) = delete;

    explicit ChatServer(std::ostream& log_ostream);
    ~ChatServer() noexcept;

    void HandleConnections();


private:
    void InitializeServerSocket();
    void AcceptNewConnection(fd_set& master, int& max_socket);
    void ProcessClientData(fd_set& master, SOCKET curr_socket);
};


SOCKET InitializeServerSocket();
