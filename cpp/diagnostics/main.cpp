/**
 * @file   main.cpp
 * @brief  Hammerhead diagnostics cli application main
 *
 */

// #include "can_defs.hpp"
#include <iostream>

#include "socketcan.hpp"

int main()
{
    can::SocketCanNode can_node;

    if (can_node.Open("can0").fault != can::SocketCanNode::Fault::NO_FAULT)
    {
        std::cerr << "Failed to open CAN socket" << std::endl;
        return -1;
    }

    if (can_node
            .RegisterRxCallback([](const can_frame* const frame) {
                std::cout << "Received CAN frame with ID: " << std::hex << frame->can_id << std::dec << std::endl;
            })
            .fault != can::SocketCanNode::Fault::NO_FAULT)
    {
        std::cerr << "Failed to register CAN receive callback" << std::endl;
        return -1;
    }

    if (can_node.Start().fault != can::SocketCanNode::Fault::NO_FAULT)
    {
        std::cerr << "Failed to start CAN node" << std::endl;
        return -1;
    }

    return 0;
}
