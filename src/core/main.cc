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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "name.h"
#include "../base/value.h"
#include "../base/function.h"
#include "../base/memory_block.h"
#include "data_block.h"
//---------------------------------------------------------------------------
const static short SRV_UDP_PORT = 6666;
const static short SRV_PORT = 8088;

//const static char* SRV_IP = "192.168.210.41";
//const char* host = "http://192.168.210.51:8088/";
const static char* SRV_IP = "192.168.50.111";
const char* host = "http://192.168.50.111:8088";
//---------------------------------------------------------------------------
std::string PointAdd(const std::string& data)
{
    std::cout << "发送到：" << SRV_IP << std::endl;

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
void DoAction(char* data, int len)
{
    std::vector<char> block(data, data+len);
    core::DataBlock data_block(std::move(block));
    if(false == data_block.Parse())
    {
        std::cout << "解析失败" << std::endl;
    }
    else
    {
        std::cout << "解析成功" << std::endl;
    }

    for(auto record : data_block.records())
    {
        json::Value value = record.get_item().ToMshtFormat();

        //修改为当前时间
        std::string name = value["userId"].val();
        value["pointTime"] = time(NULL);

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
        arr.ArrayAdd(value);
        point["trackPoint"] = arr;

        std::string snd_dat = point.ToString();
        PointAdd(snd_dat);
    }
}
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    std::cout << "ADS-B 解析程序启动，ip:" << SRV_IP << " port:" << SRV_PORT << std::endl;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
    {
        perror("Creating socket failed.\n");
        exit(1);
    }
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SRV_UDP_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sockfd, reinterpret_cast<struct sockaddr *>(&server), sizeof(server)) == -1)
    {
        perror("Bind() error.\n");
        exit(1);
    }

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    char buf[1024];
    while(1)
    {
        ssize_t num = recvfrom(sockfd, buf, sizeof(buf), 0, reinterpret_cast<struct sockaddr *>(&client), &len);
        if(num < 0)
        {
            perror("recvfrom() error.\n");
            continue;
        }

        DoAction(buf, static_cast<int>(num));
    }

    getchar();
    return 0;
}
//---------------------------------------------------------------------------
