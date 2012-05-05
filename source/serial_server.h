#ifndef SERIAL_SERVER_H
#define SERIAL_SERVER_H

#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "types_serial.h"
#include "debug_client.h"

namespace serial
{

class SerialServer : public DebugClient
{
public:
    typedef boost::function<void (const ByteArray&)> IncomingMessageHandler;

public:
    SerialServer(boost::asio::io_service& io_service) :
                 port_(io_service), timer_(io_service),
                 timeout_(io_service) {}

    virtual ~SerialServer();

    void SetDelayTime(int delay_time);
    void SetCycle(int cycle);
    void StartServerAndReceive(std::string device, int baud_rate);
    void SetMessageOut(ByteArray& message_out);
    void SetIncomingMessageHandler(IncomingMessageHandler incoming_message_handler);

private:
    IncomingMessageHandler incoming_message_handler_;
    boost::asio::serial_port port_;
    int delay_time_;
    boost::asio::deadline_timer timer_;
    boost::asio::deadline_timer timeout_;
    int cycle_;
    ByteArray message_in_;
    ByteArray message_out_;

    void Stop();
    void StartReceive();
    void StartSend(const boost::system::error_code& error);

    void HandleReceive(const boost::system::error_code& error, size_t bytes_transferred);
    void HandleSend(const boost::system::error_code& error);
    void HandleTimeout(const boost::system::error_code& error, const char* action);

    void TrySend();
};

}

#endif
