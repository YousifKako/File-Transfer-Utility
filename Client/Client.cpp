#include "Client.hpp"

#define PORT 55088
#define HOST "192.168.1.7"
//#define HOST "192.168.229.130"

Serialization::InitMetaData Client::command()
{
    /* Get Command */
    cout << "Command: ";
    std::string command;
    getline(std::cin, command);

    /* Vector to Hold Our Words */
    std::vector<std::string> tokens;

    /* Parser */
    size_t pos = 0;
    std::string token;
    while ((pos = command.find(" ")) != std::string::npos) {
        token = command.substr(0, pos);
        tokens.push_back(token);
        command.erase(0, pos + 1);
    }
    tokens.push_back(command);

    /* Chack for Valid Commands */
    if (tokens[0] == "ls" && tokens.size() == 1)
    {
        Serialization::InitMetaData data = { "ls", EMPTY_STRING, NO_FILESIZE };
        return data;
    }
    else if (tokens.size() == 2)
    {
        if (tokens[0] == "send") /* Sending Data */
        {
            boost::system::error_code ec;
            Serialization::InitMetaData data = { "send",
                tokens[1],
                boost::filesystem::file_size(tokens[1], ec)
            };

            if (ec.value() == 2)
            {
                data.filename = NO_FILE;
                data.fileSize = NO_FILESIZE;

                std::cerr << "File Does Not Exist" << std::endl;
            }

            return data;
        }
        else if (tokens[0] == "get") /* Receiving Data */
        {
            Serialization::InitMetaData data = { "get", tokens[1], NO_FILESIZE };
            return data;
        }
        else if (tokens[0] == "delete") /* Delete Data on Server */
        {
            Serialization::InitMetaData data = { "delete", tokens[1], NO_FILESIZE };
            return data;
        }
    }

    return { EMPTY_STRING, EMPTY_STRING, NO_FILESIZE };
}

void Client::recvLSFiles(tcp::socket& Socket, Serialization::InitMetaData data)
{
    Serialization::LSFile lsFile = { EMPTY_STRING, NO_FILESIZE , NO_EOF };

    while (!lsFile.eof)
    {
        Serialization::deserialize(Socket, lsFile);
        std::cout << lsFile << std::endl;
    }
}

void Client::sendFile(tcp::socket& Socket, Serialization::InitMetaData data)
{
    std::ifstream ffile;
    boost::filesystem::path const& filename = data.filename;
    if (boost::filesystem::exists(filename)) // File Exists
    {
        data.fileSize = boost::filesystem::file_size(data.filename);
        std::ifstream ffile(data.filename, std::ios::in | std::ios::binary);
        Serialization::SerializeFile file = { data.filename, data.fileSize, ffile };
        Serialization::serialize(Socket, file);
        ffile.close();
    }
    else // File Does Not Exist
    {
        ffile.close();
        Serialization::SerializeFile file = { NO_FILE, NO_FILESIZE, ffile };

        Serialization::serialize(Socket, file);
    }
}

void Client::recvFile(tcp::socket& Socket, Serialization::InitMetaData data)
{
    std::ofstream ffile;
    Serialization::DeserializeFile file = { data.filename, data.fileSize, ffile };
    Serialization::deserialize(Socket, file);
    if (file.filename == NO_FILE)
    {
        std::cerr << "File Does Not Exist on Server" << std::endl;
    }
}

void Client::deleteFile(tcp::socket& Socket, Serialization::InitMetaData data)
{
    std::uint8_t status = 2;
    Serialization::deserialize(Socket, status);

    if (status == 1)
    {
        std::cout << "File: (" << data.filename << ") Has Been Deleted From Server"
            << std::endl;
    }
    else
    {
        std::cerr << "Unable to Delete File... File Does Not Exist" << std::endl;
    }
}

void Client::sendData(tcp::socket& Socket)
{
    while (true)
    {
        try
        {
            /* Get Command */
            Serialization::InitMetaData data = this->command();

            if (data.command == "ls")
            {
                Serialization::serialize(Socket, data);
                this->recvLSFiles(Socket, data);

                // Receive ls
            }
            else if (data.command == "send")
            {
                Serialization::serialize(Socket, data);
                this->sendFile(Socket, data);

                // Send Data
            }
            else if (data.command == "get")
            {
                Serialization::serialize(Socket, data);
                this->recvFile(Socket, data);

                // Receive the Data if Exists...
                // else Receive Error
            }
            else if (data.command == "delete")
            {
                Serialization::serialize(Socket, data);
                this->deleteFile(Socket, data);
                // Receive on Success/Error...
            }
            else
            {
                std::cerr << "Not a Command" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            std::cin.get();
            exit(EXIT_FAILURE);
        }
    }
}

void Client::initClientSocket()
{
    try
    {

        io_service io_service;

        /* Socket Creation */
        tcp::socket Socket(io_service);

        /* Connection */
        Socket.connect(tcp::endpoint(ip::address::from_string(HOST), PORT));

        /* Start Communication */
        this->sendData(Socket);
    }
    catch (std::exception& e)
    {
        cerr << e.what() << endl;
    }
}

int main()
{
    Client client;
    client.initClientSocket();
}