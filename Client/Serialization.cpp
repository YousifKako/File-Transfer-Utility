#include "Serialization.hpp"
#include <algorithm>

/* Serialize A String */
void Serialization::serialize(tcp::socket& Socket, std::string const& str)
{
    std::size_t size = str.size();
    boost::asio::write(Socket, boost::asio::buffer(&size, sizeof(size)));
    boost::asio::write(Socket, boost::asio::buffer(str.data(), size));
}

void Serialization::serialize(tcp::socket& Socket, int           pod) { write(Socket, buffer(&pod, sizeof(pod))); }
void Serialization::serialize(tcp::socket& Socket, float         pod) { write(Socket, buffer(&pod, sizeof(pod))); }
void Serialization::serialize(tcp::socket& Socket, double        pod) { write(Socket, buffer(&pod, sizeof(pod))); }
void Serialization::serialize(tcp::socket& Socket, short         pod) { write(Socket, buffer(&pod, sizeof(pod))); }
void Serialization::serialize(tcp::socket& Socket, std::uint8_t  pod) { write(Socket, buffer(&pod, sizeof(pod))); }
void Serialization::serialize(tcp::socket& Socket, std::uintmax_t pod) { write(Socket, buffer(&pod, sizeof(pod))); }

void Serialization::serialize(tcp::socket& Socket, std::ifstream& file, std::uintmax_t fileSize)
{
    write(Socket, buffer(&fileSize, sizeof(fileSize)));

    char buf[MAX_DATA];
    while (file)
    {
        file.read(buf, sizeof(buf));
        write(Socket, buffer(buf, file.gcount()));
    }
}

/* Serialize Struct InitMetaData */
void Serialization::serialize(tcp::socket& Socket, Serialization::InitMetaData const& initMetaData)
{
    this->serialize(Socket, initMetaData.command);
    this->serialize(Socket, initMetaData.filename);
    this->serialize(Socket, initMetaData.fileSize);
}
/* Serialize Struct LSFile */
void Serialization::serialize(tcp::socket& Socket, Serialization::LSFile const& lsFile)
{
    this->serialize(Socket, lsFile.filename);
    this->serialize(Socket, lsFile.fileSize);
    this->serialize(Socket, lsFile.eof);
}

/* Serialize Struct File */
void Serialization::serialize(tcp::socket& Socket, Serialization::SerializeFile const& file)
{
    this->serialize(Socket, file.filename);
    this->serialize(Socket, file.file, file.fileSize);
    this->serialize(Socket, file.fileSize);
}

/* Deserialize A String */
std::string Serialization::deserialize(tcp::socket& Socket)
{
    std::size_t size = 0;
    boost::asio::read(Socket, boost::asio::buffer(&size, sizeof(size)));
    std::string ret;
    ret.resize(size);
    boost::asio::read(Socket, boost::asio::buffer(&ret[0], size));
    return ret;
}

void Serialization::deserialize(tcp::socket& Socket, int&           ret) { read(Socket, buffer(&ret, sizeof(ret))); }
void Serialization::deserialize(tcp::socket& Socket, float&         ret) { read(Socket, buffer(&ret, sizeof(ret))); }
void Serialization::deserialize(tcp::socket& Socket, double&        ret) { read(Socket, buffer(&ret, sizeof(ret))); }
void Serialization::deserialize(tcp::socket& Socket, short&         ret) { read(Socket, buffer(&ret, sizeof(ret))); }
void Serialization::deserialize(tcp::socket& Socket, std::uint8_t&  ret) { read(Socket, buffer(&ret, sizeof(ret))); }
void Serialization::deserialize(tcp::socket& Socket, std::uintmax_t& ret) { read(Socket, buffer(&ret, sizeof(ret))); }

void Serialization::deserialize(tcp::socket& Socket, std::ofstream& file)
{
    std::size_t size = 0;
    read(Socket, buffer(&size, sizeof(size)));

    char buf[MAX_DATA];
    size_t numBytes = 0;
    do {
        numBytes = read(Socket, buffer(&buf, std::min<std::size_t>(sizeof(buf), size)));
        file.write(buf, numBytes);
        size -= numBytes;
    } while (numBytes && size);

    file.close();
}

/* Deserialize Struct InitMetaData */
void Serialization::deserialize(tcp::socket& Socket, Serialization::InitMetaData& ret)
{
    ret.command = deserialize(Socket);
    ret.filename = deserialize(Socket);
    deserialize(Socket, ret.fileSize);
}

/* Deserialize Struct LSFile */
void Serialization::deserialize(tcp::socket& Socket, Serialization::LSFile& lsFile)
{
    lsFile.filename = deserialize(Socket);
    deserialize(Socket, lsFile.fileSize);
    deserialize(Socket, lsFile.eof);
}

/* Deserialize Struct File */
void Serialization::deserialize(tcp::socket& Socket, Serialization::DeserializeFile& file)
{
    file.filename = this->deserialize(Socket);
    if (file.filename != NO_FILE)
    {
        file.file.open(file.filename, std::ios::out | std::ios::trunc | std::ios::binary);
    }

    this->deserialize(Socket, file.file);
    this->deserialize(Socket, file.fileSize);
}

std::ostream& operator<<(std::ostream& out, Serialization::InitMetaData const& initData)
{
    out << "Command: " << initData.command
        << " | File Name: " << initData.filename
        << " | File Size: " << initData.fileSize;

    return out;
}

std::ostream& operator<<(std::ostream& out, Serialization::LSFile const& lsFile)
{
    out << "File Name: " << lsFile.filename
        << " | File Size: " << lsFile.fileSize;

    return out;
}