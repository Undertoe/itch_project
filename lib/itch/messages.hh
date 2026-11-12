#pragma once


#include <bit>
#include <span>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <iomanip>

#include <iostream>

enum class MessageType : char
{
    SystemEvent = 'S',
};


enum class EventCode : char
{
    StartOfDay = 'O',
    StartOfSystemHours = 'S',
    StartOfMarketHours = 'Q',
    EndOfMarketHours = 'M',
    EndOfSystemHours = 'E',
    EndOfMessages = 'C',
    
};

// shortcircuit to let us knowingly short circuit from the check by hand
template<typename T, bool short_circuit = false>
requires std::unsigned_integral<T>
void read_block(T& val, std::span<const std::byte> bytes)
{
    if constexpr (!short_circuit) 
    {
        if(sizeof(T) > bytes.size())
        {
            return T{};
        }
    }

    std::memcpy(&val, bytes.data(), sizeof(T));
    if constexpr(std::endian::native == std::endian::little)
    {
        val = std::byteswap(val);
    }
}


void track_timestamp(uint16_t & trackingNumber, std::chrono::nanoseconds& tp, std::span<const std::byte> bytes)
{
    // std::cout << "Span in messages.hh" << std::endl;
    // for(const auto & byte : bytes)
    // {
    //     uint32_t b = (uint32_t) byte;
    //     std::cout << std::hex << b << " ";
    // }

    uint64_t word{};
    read_block<uint64_t, true>(word, bytes);
    // std::cout << "word [" << std::hex << word << "]" << std::endl;

    trackingNumber = static_cast<uint16_t>(word >> 48);
    constexpr uint64_t mask_6byte = 0x0000'FFFF'FFFF'FFFFULL;
    tp = std::chrono::nanoseconds(word & mask_6byte);
}



namespace SystemMessages
{

struct SystemEventMessage
{
    static constexpr size_t MessageLength = 1 + 2 + 2 + 6 + 1;
    MessageType type{MessageType::SystemEvent};
    uint16_t stockLocate{0};
    uint16_t trackingNumber{};
    std::chrono::nanoseconds timeStamp{};
    EventCode eventCode{};

    SystemEventMessage() = default;
    SystemEventMessage(std::span<std::byte> bytes)
    {
        track_timestamp(trackingNumber, timeStamp, bytes.subspan(3, 8));
        eventCode = static_cast<EventCode>(bytes[MessageLength - 1]);
    }

    bool operator==(const SystemEventMessage& other) const = default;
};

}