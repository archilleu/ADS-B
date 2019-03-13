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

    std::string bin = 
        "150030ffa1db82ffff0121392304005d1555013347f678102e11e4000748047201a407c18f810d33b6d31d60000000ac";
    base::MemoryBlock mb = base::StringToBin(reinterpret_cast<const unsigned char*>(bin.data()), bin.size());
    std::cout << "bin:" << base::BinToString(reinterpret_cast<const unsigned char*>(mb.dat()), mb.len()) << std::endl;;
    std::vector<char> block(mb.dat(), mb.dat()+mb.len());
    core::DataBlock data_block(std::move(block));
    if(false == data_block.Parse())
    {
        std::cout << "解析失败" << std::endl;
    }

    return 0;
}
//---------------------------------------------------------------------------
