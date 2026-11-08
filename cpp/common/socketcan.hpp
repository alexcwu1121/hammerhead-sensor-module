/**
 * @file   socketcan.hpp
 * @brief  Socketcan standard can frame c++ wrapper
 *
 */

#ifndef SOCKETCAN_HPP
#define SOCKETCAN_HPP

#include <liburing.h>
#include <linux/can.h>

#include <array>
#include <cstdint>
#include <functional>
#include <string>

namespace can
{
/// @brief Basic standard CAN socketcan node with uring and callback based receive
///
/// @par Thread safety
/// - Write() may be called concurrently from any number of threads, and concurrently with
///   a Start() loop running on another thread. Each call sends one whole frame; ordering
///   between threads is unspecified.
/// - Open(), Close(), RegisterRxCallback() and destruction are NOT thread safe. They must
///   not run concurrently with any other method on the same node. In particular, join all
///   writer threads and the thread running Start() before calling Close() or destroying
///   the node; otherwise a Write() racing Close() may write into an unrelated, reused fd.
class SocketCanNode
{
public:
    /// @brief Fault codes
    enum class Fault : uint8_t
    {
        NO_FAULT = 0U,
        OPEN_FAULT,
        IF_TO_IDX_FAULT,
        BIND_FAULT,
        CLOSE_FAULT,
        ALREADY_OPEN_FAULT,
        ALREADY_CLOSED_FAULT,
        SEND_W_NOT_OPEN_FAULT,
        WRITE_FAULT,
        URING_INIT_FAULT,
        IO_BUFFER_SETUP_FAULT,
        CQE_HANDLE_FAULT,
        CALLBACK_NULL_FAULT,
        CALLBACK_ALREADY_REGISTERED_FAULT,
        NOT_OPEN_FAULT,
        SQE_FAULT,
        SUBMIT_FAULT,
        NUM_FAULTS
    };

    /// @brief Fault code and error code details
    /// @details errsv is a positive errno value. If no corresponding errno, errsv will be 0
    struct FaultDetails
    {
        Fault fault;
        int errsv;
    };

    /// @brief Rx callback type
    using RxCallback = std::function<void(const can_frame* const)>;

    /// @brief Ctor
    SocketCanNode() = default;
    ~SocketCanNode();
    SocketCanNode(const SocketCanNode&) = delete;
    SocketCanNode& operator=(const SocketCanNode&) = delete;  // NOLINT
    SocketCanNode(SocketCanNode&&) = delete;
    SocketCanNode& operator=(SocketCanNode&&) = delete;  // NOLINT

    /// @brief Open socket connection
    /// @note Not thread safe; see class thread safety notes
    /// @return FaultDetails
    [[nodiscard]] auto Open(const std::string& socketid) -> FaultDetails;

    /// @brief Close socket connection
    /// @note Not thread safe; no other method may be running or called concurrently
    /// @return FaultDetails
    [[nodiscard]] auto Close() -> FaultDetails;

    /// @brief Write a frame
    /// @note Thread safe against other Write() calls and a running Start(), but not
    ///       against Open() or Close()
    /// @param frame
    /// @return FaultDetails
    [[nodiscard]] auto Write(const can_frame& frame) -> FaultDetails;

    /// @brief Register message rx callback
    /// @note Not thread safe; register before Start() is called
    /// @param rxcallback
    /// @return FaultDetails
    [[nodiscard]] auto RegisterRxCallback(RxCallback rxcallback) -> FaultDetails;

    /// @brief Start event loop
    /// @return FaultDetails
    [[nodiscard]] auto Start() -> FaultDetails;

private:
    /// @brief Submit a multishot recv on the socket using the receive buffer ring
    /// @return FaultDetails
    [[nodiscard]] auto ArmRecv() -> FaultDetails;

    /// @brief Rx callback
    RxCallback _rxCallback = nullptr;

    /// @brief Can socket
    int _socket = -1;

    /// @brief Uring queue depth
    static constexpr uint32_t _uringQueueDepth = 64U;

    /// @brief Uring ring
    io_uring _uring {};

    /// @brief True once _uring has been successfully initialized
    bool _uringInit = false;

    /// @brief Number of receive buffers
    static constexpr uint32_t _numBufs = 64U;
    static_assert((_numBufs & (_numBufs - 1U)) == 0U, "Buffer ring size must be a power of two");

    /// @brief Size of receive buffer for standard can frame
    static constexpr uint32_t _bufSize = sizeof(can_frame);

    /// @brief Buffer group for receive buffers
    static constexpr uint32_t _bufGroup = 0;

    /// @brief Receive buffer ring
    io_uring_buf_ring* _bufRing = nullptr;

    /// @brief Receive buffers
    alignas(64) std::array<can_frame, _numBufs> _buffers = {};
};
}  // namespace can

#endif