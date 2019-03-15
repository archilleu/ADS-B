//---------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cassert>
#include "../base/function.h"
#include "../base/memory_block.h"
#include "data_block.h"
//---------------------------------------------------------------------------
void DoAction(char* data, int len)
{
    base::MemoryBlock mb = base::StringToBin(reinterpret_cast<const unsigned char*>(data), len);
    std::vector<char> block(mb.dat(), mb.dat()+mb.len());
    core::DataBlock data_block(std::move(block));
    if(false == data_block.Parse())
    {
        std::cout << "解析失败" << std::endl;
    }

    for(auto record : data_block.records())
    {
        std::string result = record.get_item().ToString();
        std::cout << result << std::endl;
        std::ofstream out("ads-b.txt", std::ios::app);
        out << result << "\r\n";

    }
}
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    const char* path = "/home/archilleu/workspace/absd/ADS-B/test/file/adsb.txt";
    FILE* fp = fopen(path, "r");
    if(nullptr == fp)
    {
        std::cout << "读取文件失败" << std::endl;
        return -1;
    }

    char line[1024];
    char buffer[1024];
    char data[1024];
    int prefix = 10;
    int idx = 0;
    while(!feof(fp))
    {
        fgets(line, sizeof(line), fp);
        if('\t' != line[0])
        {
            if(0 == idx)
                continue;

            int true_len = 0;
            for(int i=0,j=0; i<idx; i++)
            {
                if(' ' == buffer[i])
                    continue;

                data[j++] = buffer[i];
                true_len++;
            }
            std::cout << "len:" << true_len << " data:" << data << std::endl;
            DoAction(data, true_len);
            idx = 0;
            continue;
        }
        else
        {
            if(0 == idx)
            {
                char* begin = strstr(line, "1500");
                if(nullptr == begin)
                    continue;

                char* end = strstr(line, "\n");
                assert(end);

                auto size = static_cast<int>(end - begin);
                memcpy(buffer, begin, size);
                idx += size;
            }
            else
            {
                char* begin = line + prefix;
                char* end = strstr(line, "\n");
                auto size = static_cast<int>(end - begin);
                memcpy(buffer+idx, begin, size);
                idx += size;
            }
        }

    }

    fclose(fp);


    return 0;
}
//---------------------------------------------------------------------------
