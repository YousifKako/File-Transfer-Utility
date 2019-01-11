#pragma once
#include "Serialization.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;
using std::cout;
using std::cin;
using std::endl;
using std::cerr;

class Server : protected Serialization
{
private:
    std::vector<tcp::socket*> SocketList;

    Serialization::InitMetaData recvInitData(tcp::socket& Socket);
    void sendLSFiles(tcp::socket& Socket);
    void recvFile(tcp::socket& Socket, Serialization::InitMetaData initData);
    void sendFile(tcp::socket& Socket, Serialization::InitMetaData initData);
    void deleteFile(tcp::socket& Socket, Serialization::InitMetaData initData);
    void dataHandler(tcp::socket& Socket);

public:
    void initServerSocket();
};