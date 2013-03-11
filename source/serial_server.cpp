#include "serial_server.h"

#include <string>
#include <boost/bind.hpp>

#include "serial_send.h"
#include "serial_receive.h"

using namespace std;
using namespace serial;

SerialServer::SerialServer(boost::asio::io_service& io_service, string log_file) :
                           debug_(log_file), port_(io_service),
                           cycle_timer_(io_service)
{
    send_ = new SerialSend(io_service, *this);
    assert( send_ != NULL );

    receive_ = new SerialReceive(*this);
    assert( receive_ != NULL );
}

SerialServer::~SerialServer()
{
    if ( port_.is_open() )
    {
        port_.cancel();
        port_.close();
    }

    delete receive_;
    delete send_;
}

void SerialServer::StartServerAndReceive(const string device, const int baud_rate)
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

        debug_.Log() << "serialPort(" << device << "): open error = " << error << endl;
        receive_->StartReceive();
    }
    catch( exception & ex )
    {
        debug_.Log() << "serialPort(" << device << "): exception = " << ex.what() << endl;
    }
}

void SerialServer::HandleTimeout(const boost::system::error_code& error, const char* action)
{
    if ( cycle_timer_.expires_at() > boost::asio::deadline_timer::traits_type::now() )
        return;

    cycle_timer_.expires_at(boost::posix_time::pos_infin);

    send_->TrySend();

    cycle_timer_.expires_from_now(boost::posix_time::milliseconds(cycle_));
    cycle_timer_.async_wait(boost::bind(&SerialServer::HandleTimeout, this,
                            boost::asio::placeholders::error, action));
}

void SerialServer::HandleReceiveAndSend(const ByteArray& receive_data, const bool is_new_send)
{
    if ( is_new_send )
        send_->GetSendData().clear();

    if ( receive_handler_ != NULL )
        receive_handler_(receive_data);

    if ( ! send_->GetSendData().empty() )
        send_->StartSend(boost::system::error_code());
}

void SerialServer::LogData(const std::string operation, const ByteArray& data)
{
    debug_.Log() << operation;
    debug_.LogByteArray(debug_.Log(), data);
}

void SerialServer::SetDelay(const int delay)
{
    send_->SetDelay(delay);
}

void SerialServer::SetCycle(const int cycle)
{
    cycle_ = cycle;
}

void SerialServer::SendData(const ByteArray& send_data)
{
    send_->SendData(send_data);
}

void SerialServer::SetReceiveHandler(const ReceiveHandler receive_handler)
{
    receive_handler_ = receive_handler;
}

int SerialServer::GetCycle() const
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

Debug& SerialServer::GetDebug()
{
    return debug_;
}
