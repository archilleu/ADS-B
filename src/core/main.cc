//---------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cassert>
#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <list>
#include <map>
#include "name.h"
#include "../base/value.h"
#include "../base/function.h"
#include "../base/memory_block.h"
#include "data_block.h"
//---------------------------------------------------------------------------
const static short SRV_PORT = 8088;
//const static char* SRV_IP = "192.168.210.41";
const static char* SRV_IP = "192.168.50.111";
//---------------------------------------------------------------------------
std::string PointAdd(const std::string& data)
{
    const char* host = "http://192.168.50.111:8088";
    //const char* host = "http://localhost:8088/";
    const char* html = "POST %s/VisualChart/flytrail/add.do HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Cache-Control: no-cache\r\n"
        "Connection: keep-alive\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Postman-Token: 563c42d7-bda1-5496-d91d-38f378373d2c\r\n\r\n";

    std::string head = base::CombineString(html, host, host, data.length());
    std::string send_dat = head + data;
    int sockfd = socket(AF_INET,SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("socket error");
        exit(1);
    }   

    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SRV_PORT);
    inet_pton(AF_INET,SRV_IP, &serveraddr.sin_addr.s_addr);
    if(connect(sockfd,reinterpret_cast<struct sockaddr*>(&serveraddr), sizeof(serveraddr))<0)
    {
        perror("connect error");
        exit(1);
    }

    if(write(sockfd, send_dat.data(), send_dat.size())!= static_cast<ssize_t>(send_dat.size()))
    {
        perror("write error");
        exit(1);
    }

    char buffer[1024];
    ssize_t size;
    if((size=read(sockfd, buffer,sizeof(buffer)))<0)
    {
        perror("read error");
        exit(1);
    }
    std::string result(buffer, buffer+size);
    std::cout << result << std::endl;
    const char* sp = "\r\n\r\n";
    std::string json = std::string(result, result.find(sp)+strlen(sp));
    return json;
}
//---------------------------------------------------------------------------
std::map<std::string, std::list<json::Value>> name_datas;
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

        json::Value value = record.get_item().ToMshtFormat();
        std::string key = value["userId"].val();
        name_datas[key].push_back(value);
    }
}
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    std::cout << "ADS-B 解析程序启动，ip:" << SRV_IP << " port:" << SRV_PORT << std::endl;


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
    getchar();


    do
    {
        for(auto& item : name_datas)
        {
            auto& name = item.first;
            auto& list = item.second;
            if(list.empty())
                continue;

            auto val = *list.begin();
            list.pop_front();
            val["pointTime"] = time(NULL);

            auto iter = core::user_id.find(name);
            if(iter == core::user_id.end())
                continue;
            auto& id = iter->second;

            auto it = core::user_track.find(name);
            if(it == core::user_track.end())
                continue;
            auto& track = it->second;

            json::Value point(json::Value::OBJECT);
            point["userId"] = id;
            point["trackTime"] = track;
            json::Value arr(json::Value::ARRAY);
            arr.ArrayAdd(val);
            point["trackPoint"] = arr;

            std::string snd_dat = point.ToString();
            PointAdd(snd_dat);
        }

        sleep(1);

        bool finished = true;
        for(auto& item : name_datas)
        {
            if(!item.second.empty())
            {
                finished = false;
                break;
            }
        }

        if(finished)
            break;
    } while(true);

    getchar();
    return 0;
}
//---------------------------------------------------------------------------
