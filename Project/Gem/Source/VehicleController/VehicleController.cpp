/*
 * Copyright (c) Robotec.ai.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "VehicleController.h"
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace RAIControl
{

    void VehicleControllerServiceNames::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<VehicleControllerServiceNames>()
                ->Version(0)
                ->Field("continueServiceName", &VehicleControllerServiceNames::m_continueServiceName)
                ->Field("replanServiceName", &VehicleControllerServiceNames::m_replanServiceName)
                ->Field("stopServiceName", &VehicleControllerServiceNames::m_stopServiceName)
                ->Field("stateServiceName", &VehicleControllerServiceNames::m_stateServiceName)
                ->Field("flashServiceName", &VehicleControllerServiceNames::m_flashServiceName)
                ->Field("controlTopicName", &VehicleControllerServiceNames::m_controlTopicName);

            if (auto editContext = serializeContext->GetEditContext())
            {
                editContext
                    ->Class<VehicleControllerServiceNames>("VehicleControllerServiceNames", "Service names for VehicleControllerComponent")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &VehicleControllerServiceNames::m_continueServiceName,
                        "Continue service name",
                        "Service name for enforcing vehicle to continue on the planned path")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &VehicleControllerServiceNames::m_replanServiceName,
                        "Replan service name",
                        "Service name for enforcing vehicle to skip the current path and replan")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &VehicleControllerServiceNames::m_stopServiceName,
                        "Stop service name",
                        "Service name for enforcing vehicle to stop")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &VehicleControllerServiceNames::m_stateServiceName,
                        "Current state service name",
                        "Service name for getting current state of the vehicle")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &VehicleControllerServiceNames::m_flashServiceName,
                        "Flash/honk service name",
                        "Service name for enforcing vehicle to flash lights and honk")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &VehicleControllerServiceNames::m_controlTopicName,
                        "Control topic name",
                        "The control is sent over ROS 2, so it can be read`");
            }
        }
    }

    void VehicleControllerConfig::Reflect(AZ::ReflectContext* context)
    {
        VehicleControllerServiceNames::Reflect(context);

        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<VehicleControllerConfig, AZ::ComponentConfig>()
                ->Version(0)
                ->Field("serviceNames", &VehicleControllerConfig::m_serviceNames)
                ->Field("vehicleEntityId", &VehicleControllerConfig::m_vehicleEntityId)
                ->Field("vehicleSpeed", &VehicleControllerConfig::m_vehicleSpeed)
                ->Field("vehicleSteeringGain", &VehicleControllerConfig::m_vehicleSteeringGain)
                ->Field("predefinedPaths", &VehicleControllerConfig::m_predefinedPaths)
                ->Field("predefinedObstacles", &VehicleControllerConfig::m_predefinedObstacles)
                ->Field("vehicleLights", &VehicleControllerConfig::m_vehicleLights);

            if (auto editContext = serializeContext->GetEditContext())
            {
                editContext->Class<VehicleControllerConfig>("VehicleControllerConfig", "Config for VehicleControllerConfig")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &VehicleControllerConfig::m_serviceNames, "Service names", "Service names")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default, &VehicleControllerConfig::m_vehicleEntityId, "Entity Id", "Entity Id of the vehicle")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default, &VehicleControllerConfig::m_vehicleSpeed, "Speed", "Max. speed of the vehicle")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &VehicleControllerConfig::m_vehicleSteeringGain,
                        "Steering gain",
                        "Max. steering gain of the vehicle")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &VehicleControllerConfig::m_predefinedPaths,
                        "Predefined paths",
                        "Entity Ids of the predefined paths")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &VehicleControllerConfig::m_predefinedObstacles,
                        "Predefined Obstacles",
                        "Entity Ids of the predefined obstacles")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &VehicleControllerConfig::m_vehicleLights,
                        "Lights and intensities",
                        "List of the vehicle lights and max. intensities");
            }
        }
    }

} // namespace RAIControl