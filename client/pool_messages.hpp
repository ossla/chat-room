#pragma once
#include <cstring>
#include <deque>
#include <string>

class PoolMessages {
public:
    PoolMessages();
    void AddMessage(char input_buff[], const size_t msg_size, bool is_you);
    
    using iter = std::deque<std::string>::iterator;
    using citer = std::deque<std::string>::const_iterator;
    iter begin() noexcept;
    iter end() noexcept;
    [[nodiscard]] citer begin() const noexcept;
    [[nodiscard]] citer end() const noexcept;
    [[nodiscard]] citer cbegin() const noexcept;
    [[nodiscard]] citer cend() const noexcept;

    [[nodiscard]] size_t Size() const noexcept;
public:
    inline static const int MAX_LEN_MSG = 128;
    inline static const int MAX_ROWS = 30;

private:
    std::deque<std::string> pool_;

    void PushMessage(const std::string& msg);
};
