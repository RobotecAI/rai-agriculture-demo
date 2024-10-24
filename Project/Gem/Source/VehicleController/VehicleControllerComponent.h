/*
 * Copyright (c) Robotec.ai.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include "VehicleController.h"

#include <AzCore/Component/Component.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Outcome/Outcome.h>
#include <AzCore/std/utility/pair.h>
#include <rclcpp/rclcpp.hpp>
#include <std_srvs/srv/trigger.hpp>

namespace RAIControl
{
    using TriggerSrvRequest = std::shared_ptr<std_srvs::srv::Trigger::Request>;
    using TriggerSrvResponse = std::shared_ptr<std_srvs::srv::Trigger::Response>;
    using TriggerSrvHandle = std::shared_ptr<rclcpp::Service<std_srvs::srv::Trigger>>;

    class VehicleControllerComponent
        : public AZ::Component
        , public AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT(VehicleControllerComponent, "{98D5A3B5-D8C7-4D4F-8B75-4CBE47D05E08}");
        VehicleControllerComponent(){};
        ~VehicleControllerComponent(){};

        static void Reflect(AZ::ReflectContext* context);

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        void Activate() override;
        void Deactivate() override;

    private:
        //////////////////////////////////////////////////////////////////////////
        // AZ::TickBus::Handler overrides
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        //////////////////////////////////////////////////////////////////////////

        VehicleControllerConfig m_configuration;

        rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr m_continueService;
        rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr m_replanService;
        rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr m_stopService;
        rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr m_stateService;
        rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr m_flashService;

        enum class VehicleState
        {
            STOPPED = 0,
            DRIVING,
            REVERSING,
            CHANGING_PATH,
            FINISHED
        };

        VehicleState m_currentState{ VehicleState::DRIVING };
        int m_currentPath{ 0 };
        float m_currentTime{ 0.0f };
        bool m_obstacleDetected{ false };

        void LightsOn();
        void LightsOff();
        bool m_switchLights{ false };
        float m_lightsTime{ 0.0f };

        AZStd::pair<float, float> MoveVehicle();
        void DetectCollisions(const AZ::Vector3& currentPosition);
        void SwitchPath();
    };
} // namespace RAIControl
