#include "serial_server.h"

#include <iostream>
#include <string>
#include <boost/bind.hpp>

#include "debug.h"

using namespace std;
using namespace serial;

static const int kReceiveSize = 8192;

SerialServer::~SerialServer()
{
    Stop();
}

void SerialServer::Stop()
{
    if ( port_.is_open() )
    {
        port_.cancel();
        port_.close();
    }
}

void SerialServer::StartServerAndReceive(string device, int baud_rate)
{
    try
    {
        boost::system::error_code error;

        port_.open(device, error);

        port_.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));

        port_.set_option(boost::asio::serial_port_base::flow_control(
                         boost::asio::serial_port_base::flow_control::none));

        port_.set_option(boost::asio::serial_port_base::parity(
                         boost::asio::serial_port_base::parity::none));

        port_.set_option(boost::asio::serial_port_base::stop_bits(
                         boost::asio::serial_port_base::stop_bits::one));

        port_.set_option(boost::asio::serial_port_base::character_size(8));

        debug_->Log() << "serialPort(" << device << "): open error = " << error << endl;
        StartReceive();
    }
    catch( exception & ex )
    {
        debug_->Log() << "serialPort(" << device << "): exception = " << ex.what() << endl;
    }
}

static void IncreaseForReceiving(ByteArray& message, size_t size)
{
    message.reserve(size);
    message.resize(size);
}

void SerialServer::StartReceive()
{
    timeout_.expires_from_now(boost::posix_time::milliseconds(cycle_));

    IncreaseForReceiving(receive_data_, kReceiveSize);
    port_.async_read_some(boost::asio::buffer(receive_data_),
            boost::bind(&SerialServer::HandleReceive,
                        this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));

    timeout_.async_wait(boost::bind(&SerialServer::HandleTimeout,
                         this, boost::asio::placeholders::error, "receive"));
}

static void DecreaseForProcessing(ByteArray& message, size_t size)
{
    message.resize(size);
}

void SerialServer::HandleReceive(const boost::system::error_code& error, size_t bytes_transferred)
{
    /* FIXME: Split this method to sub-methods */
    debug_->Log() << "received " << bytes_transferred << ", error=" << error << endl;

    if ( error && error != boost::asio::error::message_size )
        return;

    DecreaseForProcessing(receive_data_, bytes_transferred);

    debug_->Log() << "receive:";
    debug_->LogByteArray(debug_->Log(), receive_data_);

    send_.GetSendData().clear();
    if ( receive_handler_ != NULL )
        receive_handler_(receive_data_);

    send_.StartSend(boost::system::error_code());

    /* Receive answer */
    IncreaseForReceiving(receive_data_, kReceiveSize);
    port_.async_read_some(boost::asio::buffer(receive_data_),
            boost::bind(&SerialServer::HandleReceive,
                        this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}

void SerialServer::HandleTimeout(const boost::system::error_code& error, const char* action)
{
    if ( timeout_.expires_at() > boost::asio::deadline_timer::traits_type::now() )
        return;

    timeout_.expires_at(boost::posix_time::pos_infin);

    send_.TrySend();

    timeout_.expires_from_now(boost::posix_time::milliseconds(cycle_));
    timeout_.async_wait(boost::bind(&SerialServer::HandleTimeout, this,
                        boost::asio::placeholders::error, action));
}

void SerialServer::SetDelayTime(int delay_time)
{
    delay_time_ = delay_time;
}

void SerialServer::SetCycle(int cycle)
{
    cycle_ = cycle;
}

void SerialServer::SendData(ByteArray& send_data)
{
    send_.SendData(send_data);
}

void SerialServer::SetReceiveHandler(ReceiveHandler receive_handler)
{
    receive_handler_ = receive_handler;
}

int SerialServer::GetCycle()
{
    return cycle_;
}

int SerialServer::GetDelay()
{
    return delay_time_;
}

boost::asio::serial_port& SerialServer::GetPort()
{
    return port_;
}

Debug* SerialServer::GetDebug()
{
    return debug_;
}

void SerialServer::HandleReceiveAndSend(const ByteArray& receive_data)
{
    send_.GetSendData().clear();
    if ( receive_handler_ != NULL )
        receive_handler_(receive_data);

    if ( ! send_.GetSendData().empty() )
        send_.StartSend(boost::system::error_code());
}
