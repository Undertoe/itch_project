
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

    {
        const auto& [count, type, messageData] = sub_span_read(spannedData);
        if(count == 0)
        {
            std::cout << "FAILED TO PARSE" << std::endl;
            return -1;
        }
        if(type == MessageType::SystemEvent)
        {
            SystemMessages::SystemEventMessage msg(messageData);
            std::cout << "Message acquired:\n" << msg << std::endl;
        }
    }
    
}