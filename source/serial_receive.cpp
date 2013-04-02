#include "serial_receive.h"

#include <boost/bind.hpp>

#include "serial_server.h"
#include "debug_serial.h"

using namespace std;
using namespace serial;

static const int kReceiveSize = 8192;

static void IncreaseForReceiving(ByteArray& message, size_t size)
{
    message.reserve(size);
    message.resize(size);
}

void SerialReceive::StartReceive()
{
    server_.GetCycleTimer().expires_from_now(boost::posix_time::milliseconds(server_.GetCycle()));

    IncreaseForReceiving(receive_data_, kReceiveSize);
    server_.GetPort().async_read_some(boost::asio::buffer(receive_data_),
                                      boost::bind(&SerialReceive::HandleReceive,
                                      this, boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));

    server_.GetCycleTimer().async_wait(boost::bind(&SerialServer::HandleTimeout,
                            &server_, boost::asio::placeholders::error, "receive"));
}

static void DecreaseForProcessing(ByteArray& message, const size_t size)
{
    message.resize(size);
}

void SerialReceive::HandleReceive(const boost::system::error_code& error, const size_t bytes_transferred)
{
    server_.GetDebug().Log() << "\treceived " << bytes_transferred << ", error=" << error << endl;

    if ( error && error != boost::asio::error::message_size )
        return;

    DecreaseForProcessing(receive_data_, bytes_transferred);
    server_.LogData("receive:", receive_data_);
    server_.HandleReceiveAndSend(receive_data_, true);

    ReceiveData();
}

void SerialReceive::ReceiveData()
{
    IncreaseForReceiving(receive_data_, kReceiveSize);

    server_.GetPort().async_read_some(boost::asio::buffer(receive_data_),
                                      boost::bind(&SerialReceive::HandleReceive,
                                      this, boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}
