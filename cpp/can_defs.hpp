/**
 * @file   can_defs.hpp
 * @brief  Hammerhead robot can bus definitions
 *
 */

#ifndef CAN_DEFS_HPP
#define CAN_DEFS_HPP

#include <cstdint>

/// @brief Turret Controller can definitions
namespace pc
{
/// @brief Platform Controller published CAN ID lowest index
static constexpr uint16_t PubCANIDIdx = 0x100;  // NOLINT

/// @brief Platform Controller published CAN message IDs
enum class PubCANID : uint16_t
{
    PUB_IMU_DATA_ACC_XY = PubCANIDIdx,
    PUB_IMU_DATA_ACC_Z_GYR_X,
    PUB_IMU_DATA_GYR_Y_GYR_Z,
    PUB_BATT_SOC,
    PUB_HEARTBEAT,
    PUB_FAULT_INDEX = 0x120,  // starting index for faults
    MAX_PUB_ID
};

/// @brief Platform Controller subscribed CAN ID lowest index
static constexpr uint16_t SubCANIDIdx = 0x200;  // NOLINT

/// @brief Platform Controller subscribed CAN message IDs
enum class SubCANID : uint16_t
{
    SUB_WRITE_MC1_MODE = SubCANIDIdx,
    SUB_WRITE_MC2_MODE,
    SUB_WRITE_MC1_RATE,
    SUB_WRITE_MC2_RATE,
    SUB_WRITE_MC1_DUTY,
    SUB_WRITE_MC2_DUTY,
    SUB_WRITE_MC1_DIR,
    SUB_WRITE_MC2_DIR,
    SUB_WRITE_MC1_RESET,
    SUB_WRITE_MC2_RESET,
    SUB_WRITE_IMU_RESET,
    SUB_WRITE_IMU_COMP,
    SUB_POKE_WATCHDOG,
    SUB_ENABLE_WATCHDOG,
    SUB_DISABLE_WATCHDOG,
    MAX_SUB_ID
};
}  // namespace pc

/// @brief Turret Controller can definitions
namespace tc
{
/// @brief Turret Controller published CAN ID lowest index
static constexpr uint16_t PubCANIDIdx = 0x300;  // NOLINT

/// @brief Turret Controller published CAN message IDs
enum class PubCANID : uint16_t
{
    PUB_IMU_DATA_ACC_XY = PubCANIDIdx,
    PUB_IMU_DATA_ACC_Z_GYR_X,
    PUB_IMU_DATA_GYR_Y_GYR_Z,
    PUB_HEARTBEAT,
    PUB_PAN_ENC_STATE,        // [0:3] absolute position (rad, float), [4:7] angular velocity (rad/s, float)
    PUB_TILT_ENC_STATE,       // [0:3] absolute position (rad, float), [4:7] angular velocity (rad/s, float)
    PUB_FAULT_INDEX = 0x320,  // starting index for faults
    MAX_PUB_ID
};

/// @brief Turret Controller subscribed CAN ID lowest index
static constexpr uint16_t SubCANIDIdx = 0x400;  // NOLINT

/// @brief Turret Controller subscribed CAN message IDs
enum class SubCANID : uint16_t
{
    SUB_HOME_PAN = SubCANIDIdx,
    SUB_HOME_TILT,
    SUB_ABORT_HOME_PAN,
    SUB_ABORT_HOME_TILT,
    SUB_SET_MODE_PAN,
    SUB_SET_MODE_TILT,
    SUB_SET_RATE_PAN,
    SUB_SET_RATE_TILT,
    SUB_SET_POS_PAN,
    SUB_SET_POS_TILT,
    SUB_SET_POS_KP_PAN,
    SUB_SET_POS_KI_PAN,
    SUB_SET_POS_KD_PAN,
    SUB_RATE_SLEW_PAN,
    SUB_SET_POS_KP_TILT,
    SUB_SET_POS_KI_TILT,
    SUB_SET_POS_KD_TILT,
    SUB_RATE_SLEW_TILT,
    SUB_ENABLE_TILT_INSTANT_STOP,
    SUB_ENABLE_PAN_INSTANT_STOP,
    SUB_DISABLE_TILT_INSTANT_STOP,
    SUB_DISABLE_PAN_INSTANT_STOP,
    SUB_ENABLE_TILT_STABILIZATION,
    SUB_ENABLE_PAN_STABILIZATION,
    SUB_DISABLE_TILT_STABILIZATION,
    SUB_DISABLE_PAN_STABILIZATION,
    SUB_WRITE_IMU_RESET,
    SUB_WRITE_IMU_COMP,
    SUB_POKE_WATCHDOG,
    SUB_ENABLE_WATCHDOG,
    SUB_DISABLE_WATCHDOG,
    SUB_ENABLE_PAN,
    SUB_ENABLE_TILT,
    SUB_DISABLE_PAN,
    SUB_DISABLE_TILT,
    MAX_SUB_ID
};
}  // namespace tc

#endif
