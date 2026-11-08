/**
 * @file   socketcan.cpp
 * @brief  Socketcan standard can frame c++ wrapper
 *
 */

#include "socketcan.hpp"

#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <utility>

namespace can
{
SocketCanNode::~SocketCanNode()
{
    // Attempt to close the socket and uring queue
    (void)Close();
}

auto SocketCanNode::Open(const std::string& socketid) -> FaultDetails
{
    // Check if already open
    if (_socket != -1 || _uringInit) { return {Fault::ALREADY_OPEN_FAULT, 0}; }

    // On any failure below, capture errno first, then release whatever was set up
    const auto fail = [this](Fault fault, int errsv) -> FaultDetails {
        (void)Close();
        return {fault, errsv};
    };

    // Open socket
    _socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_socket == -1) { return {Fault::OPEN_FAULT, errno}; }

    // Get index from interface name
    const unsigned int ifindex = if_nametoindex(socketid.c_str());
    if (ifindex == 0U) { return fail(Fault::IF_TO_IDX_FAULT, errno); }

    // Bind socket
    sockaddr_can addr {};
    addr.can_family = AF_CAN;
    addr.can_ifindex = static_cast<int>(ifindex);
    if (bind(_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)  // NOLINT
    {
        return fail(Fault::BIND_FAULT, errno);
    }

    // Set up uring queues
    int ret = io_uring_queue_init(_uringQueueDepth, &_uring, 0);
    if (ret != 0) { return fail(Fault::URING_INIT_FAULT, -ret); }
    _uringInit = true;

    // Set up buffers (err out-param is -errno)
    _bufRing = io_uring_setup_buf_ring(&_uring, _numBufs, _bufGroup, 0, &ret);
    if (_bufRing == nullptr) { return fail(Fault::IO_BUFFER_SETUP_FAULT, -ret); }

    // Populate buffer ring
    const auto mask = io_uring_buf_ring_mask(_numBufs);
    for (uint32_t i = 0U; i < _numBufs; i++)
    {
        io_uring_buf_ring_add(_bufRing, &_buffers[i], _bufSize, static_cast<unsigned short>(i), mask,
                              static_cast<int>(i));
    }
    io_uring_buf_ring_advance(_bufRing, _numBufs);

    return {Fault::NO_FAULT, 0};
}

auto SocketCanNode::Close() -> FaultDetails
{
    if (_socket == -1 && !_uringInit) { return {Fault::ALREADY_CLOSED_FAULT, 0}; }

    // Tear down in reverse order of setup: buffer ring, then uring, then socket
    if (_bufRing != nullptr)
    {
        (void)io_uring_free_buf_ring(&_uring, _bufRing, _numBufs, _bufGroup);
        _bufRing = nullptr;
    }

    if (_uringInit)
    {
        io_uring_queue_exit(&_uring);
        _uring = {};
        _uringInit = false;
    }

    if (_socket != -1)
    {
        const int ret = close(_socket);
        const int errsv = errno;
        _socket = -1;
        if (ret == -1) { return {Fault::CLOSE_FAULT, errsv}; }
    }

    return {Fault::NO_FAULT, 0};
}

auto SocketCanNode::Write(const can_frame& frame) -> FaultDetails
{
    // iface must be open
    if (_socket == -1) { return {Fault::SEND_W_NOT_OPEN_FAULT, 0}; }

    const auto nbytes = write(_socket, &frame, sizeof(can_frame));

    // ENOBUFS here means the tx queue is full and the caller may retry
    if (nbytes == -1) { return {Fault::WRITE_FAULT, errno}; }

    // Short write
    if (nbytes != sizeof(can_frame)) { return {Fault::WRITE_FAULT, 0}; }

    return {Fault::NO_FAULT, 0};
}

auto SocketCanNode::RegisterRxCallback(RxCallback rxcallback) -> FaultDetails
{
    if (_rxCallback != nullptr) { return {Fault::CALLBACK_ALREADY_REGISTERED_FAULT, 0}; }

    if (rxcallback == nullptr) { return {Fault::CALLBACK_NULL_FAULT, 0}; }

    _rxCallback = std::move(rxcallback);

    return {Fault::NO_FAULT, 0};
}

auto SocketCanNode::Start() -> FaultDetails
{
    if (_socket == -1 || !_uringInit || _bufRing == nullptr) { return {Fault::NOT_OPEN_FAULT, 0}; }

    if (_rxCallback == nullptr) { return {Fault::CALLBACK_NULL_FAULT, 0}; }

    // Buffer ring size mask
    const auto mask = io_uring_buf_ring_mask(_numBufs);

    // Post multishot rx
    const auto fault = ArmRecv();
    if (fault.fault != Fault::NO_FAULT) { return fault; }

    while (true)
    {
        io_uring_cqe* cqe = nullptr;
        const int ret = io_uring_wait_cqe(&_uring, &cqe);
        if (ret == -EINTR)
        {
            // retry on interruption
            continue;
        }
        if (ret < 0) { return {Fault::CQE_HANDLE_FAULT, -ret}; }

        // Copy out the completion before handing the slot back to the kernel
        const int res = cqe->res;
        const uint32_t flags = cqe->flags;
        io_uring_cqe_seen(&_uring, cqe);

        if ((flags & IORING_CQE_F_BUFFER) != 0U)
        {
            const uint32_t idx = flags >> IORING_CQE_BUFFER_SHIFT;
            if (idx >= _numBufs) { return {Fault::CQE_HANDLE_FAULT, 0}; }

            // Only hand complete standard frames to the callback
            if (res == static_cast<int>(sizeof(can_frame))) { _rxCallback(&_buffers[idx]); }

            // Return buffer to the ring
            io_uring_buf_ring_add(_bufRing, &_buffers[idx], _bufSize, static_cast<unsigned short>(idx), mask, 0);
            io_uring_buf_ring_advance(_bufRing, 1);
        }

        // ENOBUFS means the buffer ring ran dry and multishot terminated. Every buffer
        // from earlier completions has been recycled by now, so just rearm below.
        if (res < 0 && res != -ENOBUFS) { return {Fault::CQE_HANDLE_FAULT, -res}; }

        // Multishot recv has terminated, rearm
        if ((flags & IORING_CQE_F_MORE) == 0U)
        {
            const auto armed = ArmRecv();
            if (armed.fault != Fault::NO_FAULT) { return armed; }
        }
    }
}

auto SocketCanNode::ArmRecv() -> FaultDetails
{
    io_uring_sqe* sqe = io_uring_get_sqe(&_uring);
    if (sqe == nullptr) { return {Fault::SQE_FAULT, 0}; }

    io_uring_prep_recv_multishot(sqe, _socket, nullptr, 0, 0);
    sqe->flags |= IOSQE_BUFFER_SELECT;
    sqe->buf_group = _bufGroup;

    const int ret = io_uring_submit(&_uring);
    if (ret < 0) { return {Fault::SUBMIT_FAULT, -ret}; }
    if (ret == 0) { return {Fault::SUBMIT_FAULT, 0}; }

    return {Fault::NO_FAULT, 0};
}
}  // namespace can
