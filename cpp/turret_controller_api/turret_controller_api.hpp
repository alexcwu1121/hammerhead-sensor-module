/**
 * @file   turret_controller_api.hpp
 * @brief  Turret controller API
 *
 */

#ifndef TURRET_CONTROLLER_API_HPP
#define TURRET_CONTROLLER_API_HPP

/// @brief Turret Controller board API
/// @details Allow registration of callbacks to can id addresses published by turret controller. Allow setting of
/// specific can id addresses subscribed by turret controller.
class TurretControllerAPI
{
public:
    /// @brief Ctor
    TurretControllerAPI() = default;

private:
};

#endif