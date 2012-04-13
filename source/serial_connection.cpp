#include "serial_connection.h"

#include <iostream>
#include <string>
#include <boost/bind.hpp>

#include "debug.h"

using namespace std;
using namespace planar;

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
            GET_DEBUG();
            debug->Log() << "SerialConnection - error = " << ex.what() << endl;
        }
    }
}

static void PrintByteArray(const ByteArray& array)
{
    ByteArray::const_iterator i;
    for ( i = array.begin(); i != array.end(); i++ )
    {
        GET_DEBUG();
        debug->Log("0x%x", (*i));
    }
}

void SerialConnection::SendRequest(ByteArray request)
{
    Connect();

    if ( ! is_connected_ )
        return;

    GET_DEBUG();
    debug->Log() << "SerialConnection::SendRequest()" << endl;
    //PrintByteArray(request);

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

    GET_DEBUG();
    debug->Log() << "SerialConnection::ReceiveAnswer()" << endl;
    PrintByteArray(answer);

    return answer;
}
