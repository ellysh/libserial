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
        delay_timer_(io_service), server_(server) {};

    void SendData(const ByteArray& send_data);
    void StartSend(const boost::system::error_code& error);
    void TrySend();
    void SetDelay(const int delay);

    ByteArray& GetSendData();

private:
    int delay_;
    ByteArray send_data_;
    SerialServer& server_;
    boost::asio::deadline_timer delay_timer_;

    void HandleSend(const boost::system::error_code& error);
};

}

#endif
