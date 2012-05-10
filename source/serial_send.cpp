#include "serial_send.h"

#include <boost/bind.hpp>

#include "serial_server.h"
#include "debug.h"

using namespace std;
using namespace serial;

void SerialSend::SendData(ByteArray& send_data)
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

    cycle_timer_.expires_from_now(boost::posix_time::milliseconds(server_.GetCycle()));

    server_.debug_->Log() << "send:";
    server_.debug_->LogByteArray(server_.debug_->Log(), send_data_);

    server_.GetPort().async_write_some(boost::asio::buffer(send_data_),
                                       boost::bind(&SerialSend::HandleSend, this,
                                       boost::asio::placeholders::error));

    cycle_timer_.async_wait(boost::bind(&SerialServer::HandleTimeout, &server_,
                            boost::asio::placeholders::error, "send"));
}

void SerialSend::TrySend()
{
    ByteArray empty_data;
    server_.HandleReceiveAndSend(empty_data);
}

void SerialSend::HandleSend(const boost::system::error_code& error)
{
    if ( error )
        server_.debug_->Log() << "\thandle_send: " << error.message() << endl;

    send_data_.clear();
}

/* FIXME: Return the const value */
ByteArray& SerialSend::GetSendData()
{
    return send_data_;
}

void SerialSend::SetDelay(int delay)
{
    delay_ = delay;
}
