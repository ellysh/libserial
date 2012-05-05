#ifndef SERIAL_CONNECTION_H
#define SERIAL_CONNECTION_H

#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "types_serial.h"
#include "debug_client.h"

namespace serial
{

class SerialConnection : public DebugClient
{
public:
    SerialConnection(std::string device, int baud_rate, std::string log_file = "") :
        DebugClient(log_file), port_(io_service_), device_(device),
        baud_rate_(baud_rate), is_connected_(false) {}

    void SendRequest(ByteArray request);
    ByteArray ReceiveAnswer(size_t size);
    bool IsConnected();

private:
    boost::asio::io_service io_service_;
    boost::asio::serial_port port_;

    std::string device_;
    int baud_rate_;
    bool is_connected_;

    void Connect();
};

}

#endif
