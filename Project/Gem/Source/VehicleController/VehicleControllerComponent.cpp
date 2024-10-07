/*
 * Copyright (c) Robotec.ai.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "VehicleControllerComponent.h"
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <ROS2/ROS2Bus.h>

namespace RAIControl
{
    void VehicleControllerComponent::Reflect(AZ::ReflectContext* context)
    {
        VehicleControllerConfig::Reflect(context);

        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<VehicleControllerComponent>()->Version(0)->Field(
                "Configuration", &VehicleControllerComponent::m_configuration);

            if (auto editContext = serializeContext->GetEditContext())
            {
                editContext->Class<VehicleControllerComponent>("VehicleControllerComponent", "VehicleControllerComponent")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "RAIControl")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &VehicleControllerComponent::m_configuration,
                        "Configuration",
                        "Vehicle controller configuration")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly);
            }
        }
    }

    void VehicleControllerComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("ROS2Frame"));
    }

    void VehicleControllerComponent::Activate()
    {
        auto ros2Node = ROS2::ROS2Interface::Get()->GetNode();
        if (!ros2Node)
        {
            AZ_Error("VehicleControllerComponent", false, "ROS2 node is not available. ROS 2 services will not be created.");
            return;
        }

        const auto& serviceNames = m_configuration.m_serviceNames;

        m_continueService = ros2Node->create_service<std_srvs::srv::Trigger>(
            serviceNames.m_continueServiceName.c_str(),
            [this](const TriggerSrvRequest request, TriggerSrvResponse response)
            {
                m_currentState = VehicleState::DRIVING;
                response->success = true;
                response->message = "The vehicle continues the mission";
            });

        m_replanService = ros2Node->create_service<std_srvs::srv::Trigger>(
            serviceNames.m_replanServiceName.c_str(),
            [this](const TriggerSrvRequest request, TriggerSrvResponse response)
            {
                m_currentState = VehicleState::REVERSING;
                response->success = true;
                response->message = "The current mission was aborted";
            });

        m_stopService = ros2Node->create_service<std_srvs::srv::Trigger>(
            serviceNames.m_stopServiceName.c_str(),
            [this](const TriggerSrvRequest request, TriggerSrvResponse response)
            {
                m_currentState = VehicleState::STOPPED;
                response->success = true;
                response->message = "The vehicle was stopped";
            });

        m_stateService = ros2Node->create_service<std_srvs::srv::Trigger>(
            serviceNames.m_stateServiceName.c_str(),
            [this](const TriggerSrvRequest request, TriggerSrvResponse response)
            {
                AZStd::unordered_map<VehicleState, AZStd::string> stateMessages = {
                    { VehicleState::STOPPED, "STOPPED: the vehicle has stopped" },
                    { VehicleState::DRIVING, "DRIVING: the vehicle is performing a mission" },
                    { VehicleState::REVERSING, "REVERSING: the vehicle is reversing to skip the current mission" },
                    { VehicleState::CHANGING_PATH,
                      "CHANGING_PATH: the vehicle finished/aborted the current mission and will start the next one" },
                    { VehicleState::FINISHED, "FINISHED: the vehicle has stopped after finishing/aborting the last mission" }
                };

                response->success = true;
                response->message = stateMessages[m_currentState].c_str();
            });

        m_flashService = ros2Node->create_service<std_srvs::srv::Trigger>(
            serviceNames.m_flashServiceName.c_str(),
            [](const TriggerSrvRequest request, TriggerSrvResponse response)
            {
                response->success = true;
                response->message = "flash!";
            });
    }

    void VehicleControllerComponent::Deactivate()
    {
        m_continueService.reset();
        m_replanService.reset();
        m_stopService.reset();
        m_stateService.reset();
        m_flashService.reset();
    }
} // namespace RAIControl