/**
 * @file   socketcan.hpp
 * @brief  Socketcan standard can frame c++ wrapper
 *
 */

#ifndef SOCKETCAN_HPP
#define SOCKETCAN_HPP

#include <liburing.h>
#include <linux/can.h>

#include <cstdint>
#include <functional>
#include <string>

namespace can
{
/// @brief Basic standard CAN socketcan node with uring and callback based receive
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
        NUM_FAULTS
    };

    /// @brief Rx callback type
    using RxCallback = std::function<void(can_frame*)>;

    /// @brief Ctor
    SocketCanNode() = default;
    ~SocketCanNode();
    SocketCanNode(const SocketCanNode&) = delete;
    SocketCanNode& operator=(const SocketCanNode&) = delete;  // NOLINT
    SocketCanNode(SocketCanNode&&) = delete;
    SocketCanNode& operator=(SocketCanNode&&) = delete;  // NOLINT

    /// @brief Open socket connection
    /// @return Fault
    [[nodiscard]] auto Open(const std::string& socketid) -> Fault;

    /// @brief Close socket connection
    /// @return Fault
    [[nodiscard]] auto Close() -> Fault;

    /// @brief Write a frame
    /// @param frame
    /// @return Fault
    [[nodiscard]] auto Write(const can_frame& frame) -> Fault;

    /// @brief Register message rx callback
    /// @param rxcallback
    /// @return Fault
    [[nodiscard]] auto RegisterRxCallback(RxCallback rxcallback) -> Fault;

    /// @brief Start event loop
    /// @return Fault
    [[nodiscard]] auto Start() -> Fault;

private:
    /// @brief Rx callback
    RxCallback _rxCallback = nullptr;

    /// @brief Can socket
    int _socket = -1;

    /// @brief Uring queue depth
    static constexpr uint32_t _uringQueueDepth = 32U;

    /// @brief Uring ring
    io_uring _uring {};

    /// @brief Uring completion queue
    struct io_uring_cqe* _cqe = nullptr;
};
}  // namespace can

#endif