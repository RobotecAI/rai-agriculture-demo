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
        // auto node = ROS2::ROS2Interface::Get()->GetNode();
        // if (!node)
        // {
        //     AZ_Error("LightController", false, "ROS2 node is not available. Component will not be activated.");
        //     return;
        // }

        // auto* ros2Frame = ROS2::Utils::GetGameOrEditorComponent<ROS2::ROS2FrameComponent>(GetEntity());
        // if (!ros2Frame)
        // {
        //     AZ_Error(
        //         "LightController",
        //         false,
        //         "ROS2FrameComponent is not available. Component will not be "
        //         "activated.");
        //     return;
        // }

        // AZStd::string topicName = ROS2::ROS2Names::GetNamespacedName(ros2Frame->GetNamespace(), m_topicConfiguration.m_topic);
    }

    void VehicleControllerComponent::Deactivate()
    {
        // m_subscription.reset();
    }
} // namespace RAIControl