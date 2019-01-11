#pragma once
#include "Serialization.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;
using std::cout;
using std::cin;
using std::endl;
using std::cerr;

class Client : protected Serialization
{
private:
    Serialization::InitMetaData command();
    void recvLSFiles(tcp::socket& Socket, Serialization::InitMetaData data);
    void sendFile(tcp::socket& Socket, Serialization::InitMetaData data);
    void recvFile(tcp::socket& Socket, Serialization::InitMetaData data);
    void deleteFile(tcp::socket& Socket, Serialization::InitMetaData data);
    void sendData(tcp::socket& Socket);

public:
    void initClientSocket();
};