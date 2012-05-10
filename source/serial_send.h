#ifndef SERIAL_SEND_H
#define SERIAL_SEND_H

#include <boost/asio.hpp>

#include "types_serial.h"

namespace serial
{

class SerialServer;

class SerialSend
{
public:
    SerialSend(boost::asio::io_service& io_service, SerialServer& server) :
        delay_timer_(io_service), cycle_timer_(io_service), server_(server) {};
    void SendData(ByteArray& send_data);

private:
    ByteArray send_data_;
    SerialServer& server_;
    boost::asio::deadline_timer delay_timer_;
    boost::asio::deadline_timer cycle_timer_;

    void StartSend(const boost::system::error_code& error);
    void TrySend();
    void HandleSend(const boost::system::error_code& error);

    friend class SerialServer;
};

}

#endif