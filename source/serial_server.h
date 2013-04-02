#ifndef SERIAL_SERVER_H
#define SERIAL_SERVER_H

#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include <types_serial.h>
#include <debug_serial.h>

namespace serial
{

class SerialSend;
class SerialReceive;

class SerialServer
{
public:
    typedef boost::function<void (const ByteArray&)> ReceiveHandler;

public:
    SerialServer(boost::asio::io_service& io_service, const std::string log_file = "");

    virtual ~SerialServer();

    void StartServerAndReceive(const std::string device, const int baud_rate);
    void SendData(const ByteArray& send_data);

    void SetDelay(const int delay);
    void SetCycle(const int cycle);
    void SetReceiveHandler(const ReceiveHandler receive_handler);

private:
    Debug debug_;
    ReceiveHandler receive_handler_;
    boost::asio::serial_port port_;
    boost::asio::deadline_timer cycle_timer_;
    int cycle_;
    SerialSend* send_;
    SerialReceive* receive_;

    int GetCycle() const;
    boost::asio::deadline_timer& GetCycleTimer();
    boost::asio::serial_port& GetPort();
    Debug& GetDebug();

    void LogData(const std::string operation, const ByteArray& data);

    void HandleReceiveAndSend(const ByteArray& receive_data, const bool is_new_send);
    void HandleTimeout(const boost::system::error_code& error, const char* action);

    friend class SerialSend;
    friend class SerialReceive;
};

}

#endif
