#include "serial_send.h"

#include <boost/bind.hpp>

#include "serial_server.h"
#include "debug_serial.h"

using namespace std;
using namespace serial;

void SerialSend::SendData(const ByteArray& send_data)
{
    send_data_ = send_data;
}

void SerialSend::StartSend(const boost::system::error_code& error)
{
    if ( ! send_data_.empty() )
    {
        delay_timer_.expires_from_now(boost::posix_time::milliseconds(delay_));
        delay_timer_.async_wait(boost::bind(&SerialSend::StartSend, this,
                          boost::asio::placeholders::error));
    }

    server_.GetCycleTimer().expires_from_now(boost::posix_time::milliseconds(server_.GetCycle()));

    server_.LogData("send:", send_data_);
    server_.GetPort().async_write_some(boost::asio::buffer(send_data_),
                                       boost::bind(&SerialSend::HandleSend, this,
                                       boost::asio::placeholders::error));

    server_.GetCycleTimer().async_wait(boost::bind(&SerialServer::HandleTimeout, &server_,
                            boost::asio::placeholders::error, "send"));
}

void SerialSend::TrySend()
{
    server_.GetDebug().Log() << "try_send" << endl;
    ByteArray empty_data;
    server_.HandleReceiveAndSend(empty_data, false);
}

void SerialSend::HandleSend(const boost::system::error_code& error)
{
    server_.GetDebug().Log() << "\tsend error = " << error.message() << endl;
    send_data_.clear();
}

ByteArray& SerialSend::GetSendData()
{
    return send_data_;
}

void SerialSend::SetDelay(const int delay)
{
    delay_ = delay;
}
