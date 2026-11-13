#pragma once


#include <bit>
#include <ostream>
#include <span>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <iomanip>

#include <iostream>
#include <tuple>
#include <unordered_map>

enum class MessageType : char
{
    SystemEvent = 'S',
    INVALID,
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

    uint64_t word{};
    read_block<uint64_t, true>(word, bytes);

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
    SystemEventMessage(std::span<const std::byte> bytes)
    {
        track_timestamp(trackingNumber, timeStamp, bytes.subspan(3, 8));
        eventCode = static_cast<EventCode>(bytes[MessageLength - 1]);
    }

    bool operator==(const SystemEventMessage& other) const = default;
    

    friend std::ostream& operator << (std::ostream& os, const SystemEventMessage& msg)
    {
        return os << "\tTrackingNumber: " << msg.trackingNumber 
                  << "\n\tTime since Midnight: " << std::dec <<  msg.timeStamp.count() << "(ns)" 
                  << " or " << std::chrono::duration_cast<std::chrono::seconds>(msg.timeStamp).count() << "(s)"
                  << "\n\tEvent Code: " << static_cast<char>(msg.eventCode);
    }
};

}





std::unordered_map<MessageType, size_t> MessageLengths
{
    {MessageType::SystemEvent, SystemMessages::SystemEventMessage::MessageLength},
};


std::tuple<uint16_t, MessageType, std::span<const std::byte>> sub_span_read(std::span<const std::byte> data)
{
    uint16_t messageCount{};
    read_block<uint16_t, true>(messageCount, data);
    MessageType type = static_cast<MessageType>(data[2]);
    if(!MessageLengths.contains(type))
    {
        return {0, MessageType::INVALID, data};
    }
    if(static_cast<size_t>(messageCount == MessageLengths[type]))
    {
        return {messageCount, type, data.subspan(2, messageCount)};
    }
    return {0, MessageType::INVALID, data};
}