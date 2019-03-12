//---------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include "../base/function.h"
#include "../base/memory_block.h"
#include "data_block.h"
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    std::vector<char> data;
    const std::string path = "/home/archilleu/workspace/absd/ADS-B/test/file/data.bin";
    if(false == base::LoadFile(path, &data))
        return -1;

    uint8_t sign = 0x15;
    char* begin = nullptr;
    for(size_t i=0; i<data.size(); i++)
    {
        if(data[i] == sign)
        {
            if(data[i+1] != 0)
            {
                begin = data.data() + i;
                break;
            }
        }
    }
    if(nullptr == begin)
    {
        std::cout << "没有正确的数据包" << std::endl;
    }

    std::string bin = "15001CFDA011110020325E6500563F5B01278B707805B902780800A0";
    base::MemoryBlock mb = base::StringToBin(reinterpret_cast<const unsigned char*>(bin.data()), bin.size());
    //std::vector<char> block(begin, data.data()+data.size());
    std::vector<char> block(mb.dat(), mb.dat()+mb.len());
    core::DataBlock data_block(std::move(block));
    if(false == data_block.Parse())
    {
        std::cout << "解析失败" << std::endl;
    }

    return 0;
}
//---------------------------------------------------------------------------
