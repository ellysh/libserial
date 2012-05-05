#include "serial_connection.h"

#include <iostream>
#include <string>
#include <boost/bind.hpp>

#include "debug.h"

using namespace std;
using namespace serial;

void SerialConnection::Connect()
{
    if ( ! is_connected_ )
    {
        try
        {
            if ( port_.is_open() )
                port_.close();

            boost::system::error_code error;

            port_.open(device_, error);

            port_.set_option(boost::asio::serial_port_base::baud_rate(baud_rate_));

            port_.set_option(boost::asio::serial_port_base::flow_control(
                             boost::asio::serial_port_base::flow_control::none));

            port_.set_option(boost::asio::serial_port_base::parity(
                             boost::asio::serial_port_base::parity::none));

            port_.set_option(boost::asio::serial_port_base::stop_bits(
                             boost::asio::serial_port_base::stop_bits::one));

            port_.set_option(boost::asio::serial_port_base::character_size(8));

            if ( error == 0 )
                is_connected_ = true;
        }
        catch (exception& ex)
        {
            debug_->Log() << "SerialConnection - error = " << ex.what() << endl;
        }
    }
}

void SerialConnection::SendRequest(ByteArray request)
{
    Connect();

    if ( ! is_connected_ )
        return;

    debug_->Log() << "SerialConnection::SendRequest()" << endl;
    debug_->LogByteArray(debug_->Log(), request);

    try
    {
        boost::asio::write(port_, boost::asio::buffer(request));
    }
    catch ( boost::system::system_error error )
    {
        cerr << error.code() << endl;
    }
}

static void IncreaseForReceiving(ByteArray& message, size_t size)
{
    message.reserve(size);
    message.resize(size);
}

ByteArray SerialConnection::ReceiveAnswer(size_t size)
{
    Connect();

    if ( ! is_connected_ )
        return ByteArray();

    ByteArray answer;

    try
    {
        IncreaseForReceiving(answer, size);
        port_.read_some(boost::asio::buffer(answer, size));
    }
    catch ( boost::system::system_error error )
    {
        is_connected_ = false;
        return ByteArray();
    }

    debug_->Log() << "SerialConnection::ReceiveAnswer() - ";
    debug_->LogByteArray(debug_->Log(), answer);

    return answer;
}

bool SerialConnection::IsConnected()
{
    return is_connected_;
}
