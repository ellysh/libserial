#ifndef SERIAL_SERVER_H
#define SERIAL_SERVER_H

#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "types_serial.h"
#include "debug_client.h"
#include "state_client_wrap.h"

namespace serial
{

class SerialSend;
class SerialReceive;

class SerialServer : protected DebugClient, protected StateClientWrap
{
public:
    typedef boost::function<void (const ByteArray&)> ReceiveHandler;

public:
    SerialServer(boost::asio::io_service& io_service, std::string log_file = "", std::string name = "");

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
    SerialSend* send_;
    SerialReceive* receive_;

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
