#include <iostream>
#include "tinyxml.h"
#include <winsock.h>
#include <stdio.h>

using namespace std;

char* getHostIp(TiXmlDocument xml)
{
    string hostIP = xml.FirstChildElement("configuration")->
        FirstChildElement("ethernet")->FirstChildElement("hostip")->GetText();
    char* host_ip = new char[hostIP.length() + 1];
    strcpy(host_ip, hostIP.c_str());
    return host_ip;
}

int getHostPort(TiXmlDocument xml)
{
    string port = xml.FirstChildElement("configuration")->
        FirstChildElement("ethernet")->FirstChildElement("hostport")->GetText();
    return stoi(port.c_str());
}

int sendPacket(TiXmlDocument xml, char* packet)
{
    WSADATA wsa_data;
    if (WSAStartup(0x101, &wsa_data))
    {
        printf("WSAStartup error (%i)\n", WSAGetLastError());
        system("pause");
        return -1;
    }

    if (wsa_data.wVersion != 0x101)
    {
        printf("Bad WSA version (%i)\n", wsa_data.wVersion);
        system("pause");
        return -1;
    }

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        printf("socket error (%i)\n", WSAGetLastError());
        system("pause");
        return -1;
    }

    char *hostIP = getHostIp(xml);
    int host_port = getHostPort(xml);
    if (strlen(packet) > 256)
    {
        printf("too large data packet");
        return -1;
    }
    
    SOCKADDR_IN sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(host_port);
    sa.sin_addr.S_un.S_addr = inet_addr(hostIP);

    printf("wait for server...\n");
    while (true)
    {
        int connect_res = connect(s, (SOCKADDR*)&sa, sizeof(sa));
        if (!connect_res) break;
        Sleep(250);
    }
    printf("connected\n");

    char message[256];

    int message_size = sprintf
    (
        message,
        "GET [url]http://%s:%i%s[/url] HTTP/1.1\r\n"
        "Host: %s:%i\r\n"
        "Connection: Close\r\n"
        "\r\n",
        hostIP,
        host_port,
        packet,
        hostIP,
        host_port
    );

    if (send(s, message, message_size, 0) == SOCKET_ERROR)
    {
        printf("send error (%i)\n", WSAGetLastError());
        system("pause");
        return -1;
    }

    char buffer[100000];
    int buffer_size = sizeof(buffer);
    buffer_size = recv(s, buffer, buffer_size, 0);
    if (buffer_size == SOCKET_ERROR)
    {
        printf("recv error (%i)\n", WSAGetLastError());
        system("pause");
        return -1;
    }
    
    buffer[buffer_size] = '\0';
     
    printf("from_server: %s\n", buffer);
    
    closesocket(s);

    WSACleanup();
}

char* getPacket(TiXmlDocument xml)
{
    char packet[256];
    return packet;
}

int main(int argc, char* argv[])
{
    TiXmlDocument xml("config.xml");
    if (!xml.LoadFile())
    {
        printf("couldn`t open config.xml");
        return -1;
    }

	string rsText = xml.FirstChildElement("configuration")->
		FirstChildElement("rs")->FirstChildElement("port")->GetText();
	string ethernetText = xml.FirstChildElement("configuration")->
		FirstChildElement("ethernet")->FirstChildElement("port")->GetText();
	printf(ethernetText.c_str());
    printf(rsText.c_str());
    
    //todo: получать пакет с порта rs
    char *packet = getPacket(xml);
    int sendStatus = sendPacket(xml, packet);
    
	return 0;
}