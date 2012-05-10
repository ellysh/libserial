#include "serial_receive.h"

#include <boost/bind.hpp>

#include "serial_server.h"
#include "debug.h"

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
    cycle_timer_.expires_from_now(boost::posix_time::milliseconds(server_.GetCycle()));

    IncreaseForReceiving(receive_data_, kReceiveSize);
    server_.GetPort().async_read_some(boost::asio::buffer(receive_data_),
            boost::bind(&SerialReceive::HandleReceive,
                        this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));

    cycle_timer_.async_wait(boost::bind(&SerialServer::HandleTimeout,
                            &server_, boost::asio::placeholders::error, "receive"));
}

static void DecreaseForProcessing(ByteArray& message, size_t size)
{
    message.resize(size);
}

void SerialReceive::HandleReceive(const boost::system::error_code& error, size_t bytes_transferred)
{
    /* FIXME: Split this method to sub-methods */
    server_.debug_->Log() << "received " << bytes_transferred << ", error=" << error << endl;

    if ( error && error != boost::asio::error::message_size )
        return;

    DecreaseForProcessing(receive_data_, bytes_transferred);

    server_.debug_->Log() << "receive:";
    server_.debug_->LogByteArray(server_.debug_->Log(), receive_data_);

    server_.HandleReceiveAndSend(receive_data_);

    /* Receive answer */
    IncreaseForReceiving(receive_data_, kReceiveSize);
    server_.GetPort().async_read_some(boost::asio::buffer(receive_data_),
            boost::bind(&SerialReceive::HandleReceive,
                        this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}
