
#include "pool_messages.hpp"


PoolMessages::PoolMessages() = default;

void PoolMessages::AddMessage(char input_buff[], const size_t msg_size, bool is_you) {
    if (msg_size != 0) {
        if (is_you) 
            PushMessage("[You]: " + std::string(input_buff, msg_size));
        else
            PushMessage(std::string(input_buff, msg_size));

        input_buff[0] = '\0';
    }
}

inline void PoolMessages::PushMessage(const std::string& msg) {
    if (pool_.size() == MAX_ROWS) {
        pool_.pop_front();
    }
    pool_.emplace_back(msg);
}

using iter = PoolMessages::iter;
using citer = PoolMessages::citer;
iter PoolMessages::begin() noexcept {
    return pool_.begin();
}
iter PoolMessages::end() noexcept {
    return pool_.end(); 
}
[[nodiscard]] citer PoolMessages::begin() const noexcept {
    return const_cast<PoolMessages&>(*this).begin();
}
[[nodiscard]] citer PoolMessages::end() const noexcept {
    return const_cast<PoolMessages&>(*this).end();
}
[[nodiscard]] citer PoolMessages::cbegin() const noexcept {
    return pool_.cbegin();
}
[[nodiscard]] citer PoolMessages::cend() const noexcept {
    return pool_.cend(); 
}

[[nodiscard]] size_t PoolMessages::Size() const noexcept {
    return pool_.size();
}
