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
    SerialReceive(boost::asio::io_service& io_service, SerialServer& server) :
        cycle_timer_(io_service), server_(server) {};

    void StartReceive();

private:
    ByteArray receive_data_;
    SerialServer& server_;
    boost::asio::deadline_timer cycle_timer_;

    void ReceiveData();
    void HandleReceive(const boost::system::error_code& error, size_t bytes_transferred);
};

}

#endif
