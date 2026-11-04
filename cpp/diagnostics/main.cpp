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

    if (can_node.Open("can0") != can::SocketCanNode::Fault::NO_FAULT)
    {
        std::cerr << "Failed to open CAN socket" << std::endl;
        return -1;
    }

    if (can_node.RegisterRxCallback([](can_frame* frame) {
            std::cout << "Received CAN frame with ID: " << std::hex << frame->can_id << std::dec << std::endl;
        }) != can::SocketCanNode::Fault::NO_FAULT)
    {
        std::cerr << "Failed to register CAN receive callback" << std::endl;
        return -1;
    }

    if (can_node.Start() != can::SocketCanNode::Fault::NO_FAULT)
    {
        std::cerr << "Failed to start CAN node" << std::endl;
        return -1;
    }

    return 0;
}
