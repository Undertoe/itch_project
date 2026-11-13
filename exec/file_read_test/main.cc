
#include <cstddef>
#include <ios>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <itch/messages.hh>


void help()
{
    std::cout << "expected use: ./exec <PATH_TO_ITCH_FILE>" << std::endl;
}


int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        help();
    }


    std::string path(argv[1]);
    std::ifstream file(path, std::ios::binary);
    if(!file)
    {
        std::cout << "Failed to open file" << std::endl;
        return -1;
    }

    // just something big for a bit
    static constexpr size_t chunk_size = 0x1000;

    std::vector<std::byte> buffer(chunk_size);
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    auto bytes_read = file.gcount();
    std::cout << "bytes read: " << bytes_read << std::endl;
    std::cout << "First val: " << (int)buffer[0] << std::endl;
    std::cout << "first 20 vals: " << std::endl;
    for(int i = 0; i < 20; i ++)
    {
        std::cout << std::hex << (int) buffer[i] << ',';
    }
    std::cout << std::endl;

    std::cout << "Grabbing first packet: " << std::endl;

    std::span spannedData = buffer;
    std::span<const std::byte> remainingData;

    {
        const auto& readInfo = sub_span_read(spannedData);
        if(readInfo.count == 0)
        {
            std::cout << "FAILED TO PARSE" << std::endl;
            return -1;
        }
        if(readInfo.type == MessageType::SystemEvent)
        {
            SystemMessages::SystemEventMessage msg(readInfo.messageBytes);
            std::cout << "Message acquired:\n" << msg << std::endl;
        }
        remainingData = readInfo.remainingBuffer;
    }
    std::cout << "next 50 vals: " << std::endl;
    for(int i = 0; i < 50; i ++)
    {
        std::cout << std::hex << (int) remainingData[i] << ',';
    }
    std::cout << std::endl;
    std::cout << "Grabbing next packet" << std::endl;
    {
        const auto& readInfo = sub_span_read(remainingData);
        if(readInfo.count == 0)
        {
            std::cout << "FAILED TO PARSE" << std::endl;
            return -1;
        }
        if(readInfo.type == MessageType::StockDirectory)
        {
            StockMessages::StockDirectory msg(readInfo.messageBytes);
            std::cout << "Message acquired: \n" << msg << std::endl;
        }
        else 
        {
            std::cout << "GOT DIFFERENT MESSAGE TYPE: " << static_cast<char>(readInfo.type) << std::endl;
        }
        remainingData = readInfo.remainingBuffer;
    }
    std::cout << "next 50 vals: " << std::endl;
    for(int i = 0; i < 50; i ++)
    {
        std::cout << std::hex << (int) remainingData[i] << ',';
    }
    std::cout << std::endl;

    
}