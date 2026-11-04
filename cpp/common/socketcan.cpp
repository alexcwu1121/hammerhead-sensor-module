/**
 * @file   socketcan.cpp
 * @brief  Socketcan standard can frame c++ wrapper
 *
 */

#include "socketcan.hpp"

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>

namespace can
{
SocketCanNode::~SocketCanNode()
{
    // Attempt to close the socket
    (void)Close();
}

auto SocketCanNode::Open(const std::string& socketid) -> Fault
{
    // Check if already open
    if (_socket != -1) { return Fault::ALREADY_OPEN_FAULT; }

    // Open socket
    _socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_socket == -1) { return Fault::OPEN_FAULT; }

    // Get index from interface name
    ifreq ifr {};
    strncpy(ifr.ifr_name, socketid.c_str(), IFNAMSIZ - 1);  // NOLINT
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);  // NOLINT
    if (ifr.ifr_ifindex == 0) { return Fault::IF_TO_IDX_FAULT; }

    // Bind socket
    sockaddr_can addr {};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) { return Fault::BIND_FAULT; }  // NOLINT

    // Set up uring queues
    if (io_uring_queue_init(_uringQueueDepth, &_uring, 0) != 0) { return Fault::URING_INIT_FAULT; }

    // Set up multishot rx
    auto sqe = io_uring_get_sqe(&_uring);
    // Prepare the multishot recv
    io_uring_prep_recv_multishot(sqe, _socket, NULL, 0, 0);
    io_uring_submit(&_uring);

    return Fault::NO_FAULT;
}

auto SocketCanNode::Close() -> Fault
{
    if (_socket == -1) { return Fault::ALREADY_CLOSED_FAULT; }

    if (close(_socket) == -1) { return Fault::CLOSE_FAULT; }

    _socket = -1;
    return Fault::NO_FAULT;
}

auto SocketCanNode::Write(const can_frame& frame) -> Fault
{
    // iface must be open
    if (_socket == -1) { return Fault::SEND_W_NOT_OPEN_FAULT; }

    auto nbytes = write(_socket, &frame, sizeof(can_frame));

    /// TODO: revisit this
    // Check for complete write
    if (nbytes != sizeof(can_frame)) { return Fault::WRITE_FAULT; }

    return Fault::NO_FAULT;
}

auto SocketCanNode::RegisterRxCallback(RxCallback rxcallback) -> Fault
{
    if (_rxCallback != nullptr) { return Fault::ALREADY_OPEN_FAULT; }

    if (rxcallback == nullptr) { return Fault::CALLBACK_NULL_FAULT; }

    _rxCallback = rxcallback;

    return Fault::NO_FAULT;
}

auto SocketCanNode::Start() -> Fault
{
    struct io_uring_sqe* sqe;
    sqe = io_uring_get_sqe(&_uring);

    // Set up fixed io buffers
    struct iovec iov[4];
    for (int i = 0; i < 4; i++)
    {
        // hardcoding for testing only
        iov[i].iov_base = malloc(16);
        iov[i].iov_len = 16;
        memset(iov[i].iov_base, 0, 16);
    }

    if (io_uring_register_buffers(&_uring, iov, 4)) { return Fault::IO_BUFFER_SETUP_FAULT; }

    if (_rxCallback == nullptr) { return Fault::CALLBACK_NULL_FAULT; }

    while (true)
    {
        io_uring_wait_cqe(&_uring, &_cqe);

        if (_cqe->res < 0)
        {
            // Handle error
            io_uring_cqe_seen(&_uring, _cqe);
            return Fault::CQE_HANDLE_FAULT;
        }

        // Convert into CAN frame and execute callback
        int bytes_received = _cqe->res;
        int buffer_id = _cqe->flags >> IORING_CQE_BUFFER_SHIFT;
        _rxCallback(reinterpret_cast<can_frame*>(iov[buffer_id].iov_base));

        io_uring_cqe_seen(&_uring, _cqe);
    }
}
}  // namespace can
