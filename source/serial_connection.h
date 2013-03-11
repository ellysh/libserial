#ifndef SERIAL_CONNECTION_H
#define SERIAL_CONNECTION_H

#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "types_serial.h"
#include "debug.h"

namespace serial
{

class SerialConnection
{
public:
    SerialConnection(const std::string device, const int baud_rate, const std::string log_file = "") :
        port_(io_service_), debug_(log_file), device_(device),
        baud_rate_(baud_rate), is_connected_(false) {}

    void SendRequest(const ByteArray request);
    ByteArray ReceiveAnswer(const size_t size);
    bool IsConnected() const;

private:
    boost::asio::io_service io_service_;
    boost::asio::serial_port port_;

    Debug debug_;
    std::string device_;
    int baud_rate_;
    bool is_connected_;

    void Connect();
};

}

#endif
