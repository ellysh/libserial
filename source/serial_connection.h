#ifndef SERIAL_CONNECTION_H
#define SERIAL_CONNECTION_H

#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include <types_serial.h>
#include <debug_serial.h>

namespace serial
{

class SerialConnection
{
public:
    SerialConnection(const std::string device, const int baud_rate, const std::string log_file = "");

    void SendData(const ByteArray& data);
    ByteArray ReceiveData(const size_t size);

private:
    boost::asio::io_service io_service_;
    boost::asio::serial_port port_;
    Debug debug_;

    void Connect(const std::string device, const int baud_rate);
};

}

#endif
