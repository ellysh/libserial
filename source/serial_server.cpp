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

    IncreaseForReceiving(message_in_, kReceiveSize);
    port_.async_read_some(boost::asio::buffer(message_in_),
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

static void CallIncomingMessageHandlerAndLog(
        const char* inprefix,  ByteArray& msgIn,
        const char* outprefix, ByteArray& msgOut,
        SerialServer::IncomingMessageHandler incoming_message_handler,
        Debug& debug)
{
    if ( inprefix && ! msgIn.empty() )
    {
        debug.Log() << inprefix << ": ";
        debug.LogByteArray(debug.Log(), msgIn);
    }

    msgOut.clear();
    if ( incoming_message_handler != NULL )
        incoming_message_handler(msgIn);

    if ( outprefix && ! msgOut.empty() )
    {
        debug.Log() << outprefix << ": ";
        debug.LogByteArray(debug.Log(), msgOut);
    }
}

void SerialServer::HandleReceive(const boost::system::error_code& error, size_t bytes_transferred)
{
    debug_->Log() << "received " << bytes_transferred << ", error=" << error << endl;

    if ( error && error != boost::asio::error::message_size )
        return;

    DecreaseForProcessing(message_in_, bytes_transferred);
    CallIncomingMessageHandlerAndLog("\trecv", message_in_, "send", message_out_,
                                     incoming_message_handler_, *debug_);

    if ( ! message_out_.empty() )
    {
        timer_.expires_from_now(boost::posix_time::milliseconds(delay_time_));
        timer_.async_wait(boost::bind(&SerialServer::StartSend, this,
                          boost::asio::placeholders::error));
    }
    else
        StartSend(boost::system::error_code());

    IncreaseForReceiving(message_in_, kReceiveSize);
    port_.async_read_some(boost::asio::buffer(message_in_),
            boost::bind(&SerialServer::HandleReceive,
                        this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}

void SerialServer::StartSend(const boost::system::error_code& error)
{
    timeout_.expires_from_now(boost::posix_time::milliseconds(cycle_));

    port_.async_write_some(boost::asio::buffer(message_out_),
                           boost::bind(&SerialServer::HandleSend, this,
                                       boost::asio::placeholders::error));

    timeout_.async_wait(boost::bind(&SerialServer::HandleTimeout, this,
                         boost::asio::placeholders::error, "send"));
}

void SerialServer::HandleSend(const boost::system::error_code& error)
{
    if ( error )
        debug_->Log() << "\thandle_send: " << error.message() << endl;

    message_out_.clear();
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
    if ( ! message_out_.empty() )
        return;

    ByteArray message_empty;
    CallIncomingMessageHandlerAndLog(NULL, message_empty, "try_send", message_out_,
                                     incoming_message_handler_, *debug_);

    if ( ! message_out_.empty() )
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

void SerialServer::SetMessageOut(ByteArray& message_out)
{
    message_out_ = message_out;
}

void SerialServer::SetIncomingMessageHandler(IncomingMessageHandler incoming_message_handler)
{
    incoming_message_handler_ = incoming_message_handler;
}
