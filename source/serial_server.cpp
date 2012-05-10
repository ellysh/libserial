#include "serial_server.h"

#include <string>
#include <boost/bind.hpp>

#include "debug.h"

using namespace std;
using namespace serial;

SerialServer::~SerialServer()
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
        receive_.StartReceive();
    }
    catch( exception & ex )
    {
        debug_->Log() << "serialPort(" << device << "): exception = " << ex.what() << endl;
    }
}

void SerialServer::HandleTimeout(const boost::system::error_code& error, const char* action)
{
    if ( cycle_timer_.expires_at() > boost::asio::deadline_timer::traits_type::now() )
        return;

    cycle_timer_.expires_at(boost::posix_time::pos_infin);

    send_.TrySend();

    cycle_timer_.expires_from_now(boost::posix_time::milliseconds(cycle_));
    cycle_timer_.async_wait(boost::bind(&SerialServer::HandleTimeout, this,
                            boost::asio::placeholders::error, action));
}

void SerialServer::HandleReceiveAndSend(const ByteArray& receive_data, bool is_new_send)
{
    if ( is_new_send )
        send_.GetSendData().clear();

    if ( receive_handler_ != NULL )
        receive_handler_(receive_data);

    if ( ! send_.GetSendData().empty() )
        send_.StartSend(boost::system::error_code());
}

void SerialServer::LogData(std::string operation, const ByteArray& data)
{
    debug_->Log() << operation;
    debug_->LogByteArray(debug_->Log(), data);
}

void SerialServer::SetDelay(int delay)
{
    send_.SetDelay(delay);
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

boost::asio::deadline_timer& SerialServer::GetCycleTimer()
{
    return cycle_timer_;
}

boost::asio::serial_port& SerialServer::GetPort()
{
    return port_;
}

Debug* SerialServer::GetDebug()
{
    return debug_;
}
