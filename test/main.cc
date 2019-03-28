//---------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../src/base/function.h"
#include "../src/base/memory_block.h"
//---------------------------------------------------------------------------
const static short SRV_PORT = 6666;
const static char* SRV_IP = "127.0.0.1";
int fd = 0;
struct sockaddr_in addr;
//---------------------------------------------------------------------------
void InitSocket()
{
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(-1 == fd)
    {
        std::cout << "socket failed" << std::endl;
        exit(-1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SRV_PORT);
    addr.sin_addr.s_addr = inet_addr(SRV_IP);
    if(addr.sin_addr.s_addr == INADDR_NONE)
    {
        std::cout << "incorrect ip" << std::endl;
        close(fd);
        exit(-1);
    }
}
//---------------------------------------------------------------------------
void DoAction(char* data, int len)
{
    base::MemoryBlock mb = base::StringToBin(reinterpret_cast<const unsigned char*>(data), len);

    ssize_t n = sendto(fd, mb.dat(), mb.len(), 0, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    if(n != static_cast<ssize_t>(mb.len()))
    {
        perror("sendto");
    }

    sleep(1);
}
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    InitSocket();

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
            sleep(1);
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
