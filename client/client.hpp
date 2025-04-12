#pragma once
#include "../chat-room.hpp"

#include <cstring>
#include <functional>
#include <iostream>
#include <charconv>
#include <mutex>
#include <thread>


class ChatClient {
public:
    ChatClient() = delete;
    ChatClient(const ChatClient&) = delete;

    ChatClient(ChatClient&& other);
    explicit ChatClient(std::ostream& os);
    ~ChatClient() noexcept;

    void MakeConnectionWith(const char* host, const char* port);
    void Disconnect();

    void SendMessage(const char msg[], size_t size);

    /* указатель на ф-цию для UI (добавления сообщения от др. клиентов в пул) */
    std::function<void(std::string)> on_message_received;
    void StartRecvMessages();

private:
    std::ostream& log_os_;
    std::mutex peer_mutex_;

    SOCKET peer_socket_ = -1;

private:
    void RecvMessages();
};