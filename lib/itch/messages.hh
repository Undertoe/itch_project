#pragma once


#include <iostream>
#include <bit>
#include <cstddef>
#include <ostream>
#include <span>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <ranges>
#include <iostream>
#include <tuple>
#include <unordered_map>

enum class MessageType : char
{
    SystemEvent = 'S',
    StockDirectory = 'R',
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

auto normalize(std::span<const std::byte> buffer) {
    if constexpr(std::endian::native == std::endian::little)
    {
        return buffer | std::views::reverse | std::ranges::to<std::vector>();
    }
    return buffer | std::ranges::to<std::vector>();
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

enum class MarketCategory : char
{
    NasdaqGlobalSelectMarket = 'Q',
    NasdaqGlobalMarket = 'G',
    NasdaqCapitalMarket = 'S',
    NewYorkStockExchange = 'N',
    NYSEAmerican = 'A',
    NYSEArca = 'P',
    BATSZExchange = 'Z',
    InvestorsExchange = 'V',
    Unavailable = ' '
};

enum class FinancialStatusIndicator : char
{
    Deficient = 'D',
    Delinquent = 'E',
    Bankrupt = 'Q',
    Suspended = 'S',
    DeficientAndBankrupt = 'G',
    DeficientAndDelinquent = 'H',
    DelinquentAndBankrupt = 'J',
    DeficientDelinquentAndBankrupt = 'K',
    CreationOrRemdemptionsSuspended = 'C',
    Normal = 'N',
    Unavailable = ' ', 
};

enum class RoundLotsOnly : char
{
    True = 'Y',
    False = 'N'
};

// TODO: hit appendix for this later.
enum class IssueClassification : char
{
    Unavailable = ' ',
};

enum class IssueSubType : uint16_t
{
    Unavailable = ' ',
};

enum class Authenticity : char 
{
    Production = 'P',
    Test = 'T',
};

enum class ShortSaleThreshold : char
{
    Restricted = 'Y',
    NotRestricted = 'N',
    NotAvailable = ' ',
};

enum class IPOFlag : char
{
    True = 'Y', 
    Flase = 'N',
};

enum class LULDReferencePriceTier : char
{
    TierOne = '1',
    TierTwo = '2',
    unavailable = ' ',
};

enum class ETPFlag : char 
{
    True = 'Y',
    False = 'N',
};

enum class ETPInverseIndicator : char
{
    True = 'Y',
    False = 'N',
};

namespace StockMessages
{
struct StockDirectory
{
    static constexpr size_t MessageLength = 1 + 2 + 2 + 6 + 8 + 1 + 1 + 4 + 1 + 1 + 2 + 1 + 1 + 1 + 1 + 1 + 4 + 1;

    MessageType type = MessageType::StockDirectory;
    uint16_t stockLocate{};
    uint16_t trackingNumber{};
    std::chrono::nanoseconds timeStamp{};
    std::array<char, 8> stock{};
    MarketCategory marketCategory{};
    FinancialStatusIndicator statusIndicator{};
    uint32_t roundLotSize{};
    RoundLotsOnly roundLotsOnly{};
    IssueClassification issueClassification{};
    uint16_t issueSubtype{};
    // IssueSubType issueSubtype; // TODO: Need to fix this & make it work with the read bytes above.
    Authenticity authenticity{};
    ShortSaleThreshold shortSaleThreshold{};
    IPOFlag ipoFlag{}; 
    LULDReferencePriceTier luldReferencePriceTier{};
    ETPFlag etpFlag{};
    uint32_t etpLeverageFactor{};
    ETPInverseIndicator etpInverseIndicator{};


    StockDirectory() = default;

    StockDirectory(std::span<const std::byte> bytes)
    {
        read_block<uint16_t, true>(stockLocate, bytes.subspan(1));
        track_timestamp(trackingNumber, timeStamp, bytes.subspan(3, 8));

        std::memcpy(stock.data(), bytes.subspan(11, stock.size()).data(), stock.size());
        
        marketCategory = static_cast<MarketCategory>(bytes[19]);
        statusIndicator = static_cast<FinancialStatusIndicator>(bytes[20]);
        read_block<uint32_t, true>(roundLotSize, bytes.subspan(21));
        roundLotsOnly = static_cast<RoundLotsOnly>(bytes[25]);
        issueClassification = static_cast<IssueClassification>(bytes[26]);
        
        read_block<uint16_t, true>(issueSubtype, bytes.subspan(27));
        authenticity = static_cast<Authenticity>(bytes[29]);
        shortSaleThreshold = static_cast<ShortSaleThreshold>(bytes[30]);
        ipoFlag = static_cast<IPOFlag>(bytes[31]);
        luldReferencePriceTier = static_cast<LULDReferencePriceTier>(bytes[32]);
        etpFlag = static_cast<ETPFlag>(bytes[33]);

        
        read_block<uint32_t, true>(etpLeverageFactor, bytes.subspan(34));
        etpInverseIndicator = static_cast<ETPInverseIndicator>(bytes[38]);
    }

    std::vector<std::byte> networkEncode() const
    {
        std::vector<std::byte> bytes;
        bytes.push_back(static_cast<std::byte>(type));

        auto bytesTmp = std::as_bytes(std::span(&stockLocate, 1));
        auto vec = normalize(bytesTmp);
        bytes.append_range(vec);

        bytesTmp = std::as_bytes(std::span(&trackingNumber, 1));
        vec = normalize(bytesTmp);
        bytes.append_range(vec);
        
        auto val = timeStamp.count();
        bytesTmp = std::as_bytes(std::span(&val, 1));
        vec = normalize(bytesTmp);
        vec.erase(vec.begin(), vec.begin() + 2);
        bytes.append_range(vec);

        bytes.append_range(std::as_bytes(std::span(stock)));
        
        bytes.push_back(static_cast<std::byte>(marketCategory));
        bytes.push_back(static_cast<std::byte>(statusIndicator));

        bytesTmp = std::as_bytes(std::span(&roundLotSize, 1));
        vec = normalize(bytesTmp);
        bytes.append_range(vec);

        bytes.push_back(static_cast<std::byte>(roundLotsOnly));
        bytes.push_back(static_cast<std::byte>(issueClassification));

        bytesTmp = std::as_bytes(std::span(&issueSubtype, 1));
        vec = normalize(bytesTmp);
        bytes.append_range(vec);

        bytes.push_back(static_cast<std::byte>(authenticity));
        bytes.push_back(static_cast<std::byte>(shortSaleThreshold));
        bytes.push_back(static_cast<std::byte>(ipoFlag));
        bytes.push_back(static_cast<std::byte>(luldReferencePriceTier));
        bytes.push_back(static_cast<std::byte>(etpFlag));
        
        bytesTmp = std::as_bytes(std::span(&etpLeverageFactor, 1));
        vec = normalize(bytesTmp);
        bytes.append_range(vec);

        bytes.push_back(static_cast<std::byte>(etpInverseIndicator));

        return bytes;
    }

    bool operator==(const StockDirectory& other) const = default;



    friend std::ostream& operator << (std::ostream& os, const StockDirectory& msg)
    {
        return os << "\n\tStockLocate: " << msg.stockLocate 
                  << "\n\tTrackingNumber: " << msg.trackingNumber 
                  << "\n\tTime since Midnight: " << std::dec <<  msg.timeStamp.count() << "(ns)" 
                  << " or " << std::chrono::duration_cast<std::chrono::seconds>(msg.timeStamp).count() << "(s)"
                  << "\n\tStockName [" << std::string_view(msg.stock.begin(), msg.stock.end()) << "]"
                  << "\n\tMarketCategory: " << static_cast<char>(msg.marketCategory)
                  << "\n\tFinancialStatusIndicator: " << static_cast<char>(msg.statusIndicator)
                  << "\n\tRoundLotSize: " << msg.roundLotSize
                  << "\n\tRoundLotsOnly: " << static_cast<char>(msg.roundLotsOnly)
                  << "\n\tIssueClassification: " << static_cast<char>(msg.issueClassification)
                  << "\n\tIssueSubtype: " << msg.issueSubtype
                  << "\n\tauthenticity: " << static_cast<char>(msg.authenticity)
                  << "\n\tshortSaleThreshold: " << static_cast<char>(msg.shortSaleThreshold)
                  << "\n\tipoFlag: " << static_cast<char>(msg.ipoFlag)
                  << "\n\tluldReferencePriceTier: " << static_cast<char>(msg.luldReferencePriceTier)
                  << "\n\tetpFlag: " << static_cast<char>(msg.etpFlag)
                  << "\n\tetpLeverageFactor: " << msg.etpLeverageFactor
                  << "\n\tetpInverseIndicator: " << static_cast<char>(msg.etpInverseIndicator);
    }

};
}





std::unordered_map<MessageType, size_t> MessageLengths
{
    {MessageType::SystemEvent, SystemMessages::SystemEventMessage::MessageLength},
    {MessageType::StockDirectory, StockMessages::StockDirectory::MessageLength},
};

struct ReadReturn
{
    uint16_t count{0};
    MessageType type{MessageType::INVALID};
    std::span<const std::byte> messageBytes{};
    std::span<const std::byte> remainingBuffer{};
};

ReadReturn sub_span_read(std::span<const std::byte> data)
{
    uint16_t messageCount{};
    read_block<uint16_t, true>(messageCount, data);
    MessageType type = static_cast<MessageType>(data[2]);
    if(!MessageLengths.contains(type))
    {
        return {};
    }
    if(static_cast<size_t>(messageCount == MessageLengths[type]))
    {
        return {messageCount, type, data.subspan(2, messageCount), data.subspan(2 + messageCount)};
    }
    return {};
}