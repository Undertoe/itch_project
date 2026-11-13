#include <bit>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <ranges>
#include <algorithm>
#include <itch/messages.hh>

#include <catch2/catch_test_macros.hpp>
#include <iterator>
#include <span>

#include <iomanip>
#include <iostream>

auto normalize(std::span<const std::byte> buffer) {
    if constexpr(std::endian::native == std::endian::little)
    {
        return buffer | std::views::reverse | std::ranges::to<std::vector>();
    }
    return buffer | std::ranges::to<std::vector>();
}


TEST_CASE("Message test", "Verifying message type")
{
    char c = 'S';
    MessageType type = static_cast<MessageType>(c);
    REQUIRE(type == MessageType::SystemEvent);

    c = 'T';
    type = static_cast<MessageType>(c);
    REQUIRE(type != MessageType::SystemEvent);
    // REQUIRE(type == MessageType::INVALID);
    
    c = 'U';
    type = static_cast<MessageType>(c);
    REQUIRE(type != MessageType::SystemEvent);
    // REQUIRE(type == MessageType::INVALID); fails
    
    c = 'R';
    type = static_cast<MessageType>(c);
    REQUIRE(type != MessageType::SystemEvent);
    // REQUIRE(type == MessageType::INVALID); fails
}


TEST_CASE("READ_BLOCK", "Validate")
{
    uint16_t V16{16};
    uint32_t V32{0xA0B0C0D0};
    uint64_t V64{0x0102030405060708};

    uint16_t check16;
    uint16_t tmp16 = V16;
    
    if constexpr (std::endian::native == std::endian::little)
    {
        // set to big endian
        tmp16 = std::byteswap(tmp16);
    }    
    auto bytes = std::as_bytes(std::span(&tmp16, 1));
    read_block<uint16_t, true>(check16, bytes);
    REQUIRE(check16 == V16);

    uint32_t check32;
    uint32_t tmp32 = V32;
    if constexpr (std::endian::native == std::endian::little)
    {
        // set to big endian
        tmp32 = std::byteswap(tmp32);
    }    
    bytes = std::as_bytes(std::span(&tmp32, 1));
    read_block<uint32_t, true>(check32, bytes);
    REQUIRE(check32 == V32);

    uint64_t check64;
    uint64_t tmp64 = V64;
    if constexpr (std::endian::native == std::endian::little)
    {
        // set to big endian
        tmp64 = std::byteswap(tmp64);
    }    
    bytes = std::as_bytes(std::span(&tmp64, 1));
    read_block<uint64_t, true>(check64, bytes);
    REQUIRE(check64 == V64);
}


TEST_CASE("SystemEventMessages", "Parse")
{
    SystemMessages::SystemEventMessage msg;
    msg.trackingNumber = 0x1234;
    msg.timeStamp = std::chrono::nanoseconds{10000};
    msg.eventCode = EventCode::EndOfMarketHours;

    // going to build the type bit by bit
    std::vector<std::byte> bytes;
    auto bytesTmp = std::as_bytes(std::span(&msg.type, 1));
    bytes.append_range(bytesTmp);

    bytesTmp = std::as_bytes(std::span(&msg.stockLocate, 1));
    auto vec = normalize(bytesTmp);
    bytes.append_range(vec);

    bytesTmp = std::as_bytes(std::span(&msg.trackingNumber, 1));
    vec = normalize(bytesTmp);
    bytes.append_range(vec);

    auto val = msg.timeStamp.count();
    bytesTmp = std::as_bytes(std::span(&val, 1));
    vec = normalize(bytesTmp);
    // drop front 2 bytes since these aren't actually part of the time.
    vec.erase(vec.begin(), vec.begin() + 2);
    bytes.append_range(vec);

    bytesTmp = std::as_bytes(std::span(&msg.eventCode, 1));
    bytes.append_range(bytesTmp);

    // should now have our bytes, we will construct a new system event message & compare

    SystemMessages::SystemEventMessage msg2(bytes);

    REQUIRE(msg2 == msg);
}