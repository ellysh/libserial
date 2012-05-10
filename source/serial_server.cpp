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

    send_data_.clear();
    if ( receive_handler_ != NULL )
        receive_handler_(receive_data_);

    /* Send new data */
    if ( ! send_data_.empty() )
    {
        timer_.expires_from_now(boost::posix_time::milliseconds(delay_time_));
        timer_.async_wait(boost::bind(&SerialServer::StartSend, this,
                          boost::asio::placeholders::error));
    }
    else
        StartSend(boost::system::error_code());

    /* Receive answer */
    IncreaseForReceiving(receive_data_, kReceiveSize);
    port_.async_read_some(boost::asio::buffer(receive_data_),
            boost::bind(&SerialServer::HandleReceive,
                        this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}

void SerialServer::StartSend(const boost::system::error_code& error)
{
    timeout_.expires_from_now(boost::posix_time::milliseconds(cycle_));

    debug_->Log() << "send" << ": ";
    debug_->LogByteArray(debug_->Log(), send_data_);

    port_.async_write_some(boost::asio::buffer(send_data_),
                           boost::bind(&SerialServer::HandleSend, this,
                                       boost::asio::placeholders::error));

    timeout_.async_wait(boost::bind(&SerialServer::HandleTimeout, this,
                         boost::asio::placeholders::error, "send"));
}

void SerialServer::HandleSend(const boost::system::error_code& error)
{
    if ( error )
        debug_->Log() << "\thandle_send: " << error.message() << endl;

    send_data_.clear();
}

void SerialServer::HandleTimeout(const boost::system::error_code& error, const char* action)
{
    if ( timeout_.expires_at() > boost::asio::deadline_timer::traits_type::now() )
        return;

    timeout_.expires_at(boost::posix_time::pos_infin);

    TrySend();

    timeout_.expires_from_now(boost::posix_time::milliseconds(cycle_));
    timeout_.async_wait(boost::bind(&SerialServer::HandleTimeout, this,
                        boost::asio::placeholders::error, action));
}

void SerialServer::TrySend()
{
    if ( ! send_data_.empty() )
        return;

    send_data_.clear();
    if ( receive_handler_ != NULL )
    {
        ByteArray empty_data;
        receive_handler_(empty_data);
    }

    if ( ! send_data_.empty() )
        StartSend(boost::system::error_code());
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
    send_data_ = send_data;
}

void SerialServer::SetReceiveHandler(ReceiveHandler receive_handler)
{
    receive_handler_ = receive_handler;
}
