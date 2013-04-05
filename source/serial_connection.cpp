#include "serial_connection.h"

#include <iostream>
#include <string>
#include <boost/bind.hpp>

using namespace std;
using namespace serial;

SerialConnection::SerialConnection(const string device, const int baud_rate, const string log_file) :
    port_(io_service_), debug_(log_file)
{
    Connect(device, baud_rate);
}

void SerialConnection::Connect(const string device, const int baud_rate)
{
    try
    {
        if ( port_.is_open() )
            port_.close();

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

        if ( error != 0 )
        {
            debug_.Log() << "SerialConnection - error = " << error << endl;
            exit(1);
        }
    }
    catch (exception& ex)
    {
        debug_.Log() << "SerialConnection - error = " << ex.what() << endl;
        exit(1);
    }
}

void SerialConnection::SendData(const ByteArray& data)
{
#ifdef __DEBUG__
    debug_.Log() << "SerialConnection::SendData()" << endl;
    debug_.LogByteArray(debug_.Log(), data);
#endif

    try
    {
        boost::asio::write(port_, boost::asio::buffer(data));
    }
    catch ( boost::system::system_error error )
    {
        debug_.Log() << "SerialConnection - error = " << error.code() << endl;
    }
}

static void IncreaseForReceiving(ByteArray& message, const size_t size)
{
    message.reserve(size);
    message.resize(size);
}

ByteArray SerialConnection::ReceiveData(const size_t size)
{
    ByteArray answer;
    size_t bytes_transferred;

    try
    {
        IncreaseForReceiving(answer, size);
        bytes_transferred = port_.read_some(boost::asio::buffer(answer, size));
    }
    catch ( boost::system::system_error error )
    {
        debug_.Log() << "SerialConnection - error = " << error.what() << endl;
        exit(1);
    }

    answer.resize(bytes_transferred);

#ifdef __DEBUG__
    debug_.Log() << "SerialConnection::ReceiveAnswer() - ";
    debug_.LogByteArray(debug_.Log(), answer);
#endif

    return answer;
}
