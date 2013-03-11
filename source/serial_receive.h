#ifndef SERIAL_RECEIVE_H
#define SERIAL_RECEIVE_H

#include <boost/asio.hpp>

#include "types_serial.h"

namespace serial
{

class SerialServer;

class SerialReceive
{
public:
    SerialReceive(SerialServer& server) : server_(server) {};

    void StartReceive();

private:
    ByteArray receive_data_;
    SerialServer& server_;

    void ReceiveData();
    void HandleReceive(const boost::system::error_code& error, const size_t bytes_transferred);
};

}

#endif
