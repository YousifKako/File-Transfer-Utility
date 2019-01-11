#pragma once
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#define EMPTY_STRING ""
#define NO_FILE      "NO_FILE"
#define NO_FILESIZE  0
#define DIRECTORY    1
#define NO_EOF       0
#define YES_EOF      1
#define MAX_DATA     8192

using namespace boost::asio;
using namespace boost::asio::ip;

class Serialization
{
public:
    struct InitMetaData
    {
        std::string command;
        std::string filename;
        std::uintmax_t fileSize;
    };

    struct LSFile
    {
        std::string filename;
        std::uintmax_t fileSize;
        int eof;
    };

    struct SerializeFile
    {
        std::string filename;
        std::uintmax_t fileSize;
        std::ifstream& file;
    };

    struct DeserializeFile
    {
        std::string filename;
        std::uintmax_t fileSize;
        std::ofstream& file;
    };

    struct DeleteStatus
    {
        std::uint8_t status;
    };

private:
    void serialize(tcp::socket& Socket, std::string const& str);
    void serialize(tcp::socket& Socket, int                pod);
    void serialize(tcp::socket& Socket, float              pod);
    void serialize(tcp::socket& Socket, double             pod);
    void serialize(tcp::socket& Socket, short              pod);
    void serialize(tcp::socket& Socket, std::uintmax_t      pod);
    void serialize(tcp::socket& Socket, std::ifstream&     file, std::uintmax_t const fileSize);

    std::string deserialize(tcp::socket& Socket);
    void deserialize(tcp::socket& Socket, int&            ret);
    void deserialize(tcp::socket& Socket, float&          ret);
    void deserialize(tcp::socket& Socket, double&         ret);
    void deserialize(tcp::socket& Socket, short&          ret);
    void deserialize(tcp::socket& Socket, std::uintmax_t&  ret);
    void deserialize(tcp::socket& Socket, std::ofstream&  file);
    
protected:
    void serialize(tcp::socket& Socket, std::uint8_t       pod);
    void serialize(tcp::socket& Socket, InitMetaData const& initMetaData);
    void serialize(tcp::socket& Socket, LSFile const& lsFile);
    void serialize(tcp::socket& Socket, SerializeFile const& file);
    void deserialize(tcp::socket& Socket, std::uint8_t&   ret);
    void deserialize(tcp::socket& Socket, InitMetaData& ret);
    void deserialize(tcp::socket& Socket, LSFile& lsFile);
    void deserialize(tcp::socket& Socket, DeserializeFile& File);

    friend std::ostream& operator<<(std::ostream& out, InitMetaData const& initData);
    friend std::ostream& operator<<(std::ostream& out, LSFile const& lsFile);
};