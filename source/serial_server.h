#ifndef SERIAL_SERVER_H
#define SERIAL_SERVER_H

#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "types_serial.h"
#include "debug_client.h"
#include "serial_send.h"

namespace serial
{

class SerialServer : public DebugClient
{
public:
    typedef boost::function<void (const ByteArray&)> ReceiveHandler;

public:
    SerialServer(boost::asio::io_service& io_service, std::string log_file = "") :
                 DebugClient(log_file), port_(io_service), timer_(io_service),
                 timeout_(io_service), send_(io_service, *this) {}

    virtual ~SerialServer();

    void SetDelayTime(int delay_time);
    void SetCycle(int cycle);
    void StartServerAndReceive(std::string device, int baud_rate);
    void SendData(ByteArray& send_data);
    void SetReceiveHandler(ReceiveHandler receive_handler);

    void HandleReceiveAndSend(const ByteArray& receive_data);
    int GetCycle();
    int GetDelay();
    boost::asio::serial_port& GetPort();
    Debug* GetDebug();

private:
    ReceiveHandler receive_handler_;
    boost::asio::serial_port port_;
    int delay_time_;
    boost::asio::deadline_timer timer_;
    boost::asio::deadline_timer timeout_;
    int cycle_;
    ByteArray receive_data_;
    SerialSend send_;

    void Stop();
    void StartReceive();

    void HandleReceive(const boost::system::error_code& error, size_t bytes_transferred);
    void HandleTimeout(const boost::system::error_code& error, const char* action);

    friend class SerialSend;
    friend class SerialReceive;
};

}

#endif
