#ifndef SERIAL_SERVER_H
#define SERIAL_SERVER_H

#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "types_serial.h"
#include "debug_client.h"
#include "serial_send.h"
#include "serial_receive.h"

namespace serial
{

class SerialServer : public DebugClient
{
public:
    typedef boost::function<void (const ByteArray&)> ReceiveHandler;

public:
    SerialServer(boost::asio::io_service& io_service, std::string log_file = "") :
                 DebugClient(log_file), port_(io_service), cycle_timer_(io_service),
                 send_(io_service, *this), receive_(*this) {}

    virtual ~SerialServer();

    void StartServerAndReceive(std::string device, int baud_rate);
    void SendData(const ByteArray& send_data);

    void SetDelay(int delay);
    void SetCycle(int cycle);
    void SetReceiveHandler(ReceiveHandler receive_handler);

private:
    ReceiveHandler receive_handler_;
    boost::asio::serial_port port_;
    boost::asio::deadline_timer cycle_timer_;
    int cycle_;
    SerialSend send_;
    SerialReceive receive_;

    int GetCycle();
    boost::asio::deadline_timer& GetCycleTimer();
    boost::asio::serial_port& GetPort();
    Debug* GetDebug();
    void LogData(std::string operation, const ByteArray& data);

    void HandleReceiveAndSend(const ByteArray& receive_data, bool is_new_send);
    void HandleTimeout(const boost::system::error_code& error, const char* action);

    friend class SerialSend;
    friend class SerialReceive;
};

}

#endif
