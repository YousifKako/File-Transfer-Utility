#include "Server.hpp"
#include <boost/range/iterator_range.hpp>
#include <boost/exception/error_info.hpp>

#define PORT 55088

Serialization::InitMetaData Server::recvInitData(tcp::socket& Socket)
{
    Serialization::InitMetaData initData = { EMPTY_STRING, EMPTY_STRING, NO_FILESIZE };
    Serialization::deserialize(Socket, initData);

    return initData;
}

void Server::sendLSFiles(tcp::socket& Socket)
{
    boost::filesystem::path p = ".";

    ptrdiff_t numRegular = std::count_if(
        boost::filesystem::directory_iterator(p),
        boost::filesystem::directory_iterator(),
        static_cast<bool(*)(const boost::filesystem::path&)>(boost::filesystem::is_regular_file)
    );

    ptrdiff_t numDirectories = std::count_if(
        boost::filesystem::directory_iterator(p),
        boost::filesystem::directory_iterator(),
        static_cast<bool(*)(const boost::filesystem::path&)>(boost::filesystem::is_directory)
    );

    ptrdiff_t totalFiles = numRegular + numDirectories;

    /* Get All Filenames and Their Size in the Current Directory */
    for (boost::filesystem::directory_entry& entry
        : boost::make_iterator_range(boost::filesystem::directory_iterator(p), {}))
    {
        int ifEof = (totalFiles - 1) ? NO_EOF : YES_EOF;

        /* Convert boost::filesystem::directory_entry ==> std::string */
        std::string stringEntry = entry.path().string();

        if (boost::filesystem::is_regular(entry)) // If File is Regular
        {
            Serialization::LSFile lsFile = {
                stringEntry,
                boost::filesystem::file_size(entry),
                ifEof
            };
            Serialization::serialize(Socket, lsFile);
        }
        else if (boost::filesystem::is_directory(entry)) // If File is Directory
        {
            Serialization::LSFile lsFile = {
                stringEntry,
                DIRECTORY,
                ifEof
            };
            Serialization::serialize(Socket, lsFile);
        }

        if (!ifEof)
            totalFiles--;
    }
}

void Server::recvFile(tcp::socket& Socket, Serialization::InitMetaData initData)
{
    std::ofstream ffile;
    Serialization::DeserializeFile file = { initData.filename, initData.fileSize, ffile };
    Serialization::deserialize(Socket, file);

    if (file.filename == NO_FILE)
    {
        std::cerr << "File Does Not Exist" << std::endl;
    }
}

void Server::sendFile(tcp::socket& Socket, Serialization::InitMetaData initData)
{
    std::ifstream ffile;
    boost::filesystem::path const& filename = initData.filename;
    if (boost::filesystem::exists(filename)) // File Exists
    {
        initData.fileSize = boost::filesystem::file_size(filename);
        std::ifstream ffile(initData.filename, std::ios::in | std::ios::binary);
        Serialization::SerializeFile file = { initData.filename, initData.fileSize, ffile };
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

void Server::deleteFile(tcp::socket& Socket, Serialization::InitMetaData initData)
{
    std::uint8_t status = 2;
    boost::filesystem::path const& filename = initData.filename;
    if (boost::filesystem::exists(filename)) // File Exists
    {
        remove(initData.filename.c_str());
        std::uint8_t status = 1;
        Serialization::serialize(Socket, status);
    }
    else // File Does Not Exist
    {
        std::uint8_t status = 0;
        Serialization::serialize(Socket, status);
    }
}

void Server::dataHandler(tcp::socket& Socket)
{
    while (true)
    {
        try
        {
            /* Receive Initial Data */
            Serialization::InitMetaData initData = recvInitData(Socket);

            std::cout << "IP: " << Socket.remote_endpoint().address()
                << " | Port: " << Socket.remote_endpoint().port()
                << initData << std::endl;


            if (initData.command == "ls")
            {
                this->sendLSFiles(Socket);
                // Send ls
            }
            else if (initData.command == "send") /* Receiving Data */
            {
                this->recvFile(Socket, initData);
                // Receive Data
            }
            else if (initData.command == "get") /* Sending Data */
            {
                this->sendFile(Socket, initData);
                // Send Data if Exists...
                // else Send Error
            }
            else if (initData.command == "delete") /* Delete Data on Server */
            {
                this->deleteFile(Socket, initData);
                // Delete File if Exists and Send Success
                // else Send Failure
            }
            else
            {
                std::cerr << "This Functionality is Not Implemented" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            bool status = (std::string)e.what() == "read: End of file";

            /* Client Disconnected */
            if (status)
            {
                std::cerr << "Disconnected: " << Socket.remote_endpoint() << std::endl;
                Socket.close();
                return;
            }

            /* Other Error */
            std::cerr << "***" << e.what() << std::endl;
            std::cin.get();
            exit(EXIT_FAILURE);
        }
    }
}

void Server::initServerSocket()
{
    while (true)
    {
        try
        {
            io_service io_service;

            /* Initialize to Accept a TCP connection on PORT */
            tcp::acceptor AcceptConnection(io_service, tcp::endpoint(tcp::v6(), PORT));

            /* Socket Creation */
            tcp::socket Socket(io_service);


            std::cout << "Waiting for Client..." << std::endl;

            /* Listen for Connections */
            AcceptConnection.accept(Socket);

            std::cout << "Connection Established: " 
                << Socket.remote_endpoint() << std::endl;

            /* Add the Connection to SocketList */
            this->SocketList.push_back(&Socket);

            /* Start Data Handler */
            this->dataHandler(Socket);

        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }

    }
}

int main()
{
    Server server;
    server.initServerSocket();
}