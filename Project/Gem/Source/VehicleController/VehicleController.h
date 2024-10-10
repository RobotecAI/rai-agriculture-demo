/*
 * Copyright (c) Robotec.ai.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/base.h>
#include <AzCore/std/containers/unordered_map.h>
#include <AzCore/std/containers/vector.h>
#include <AzCore/std/string/string.h>

namespace RAIControl
{

    struct VehicleControllerServiceNames
    {
        AZ_TYPE_INFO(VehicleControllerServiceNames, "{E06548EE-F336-4990-ACE1-96572CE57B64}");
        AZStd::string m_continueServiceName = "continue";
        AZStd::string m_replanServiceName = "replan";
        AZStd::string m_stopServiceName = "stop";
        AZStd::string m_stateServiceName = "current_state";
        AZStd::string m_flashServiceName = "flash";

        static void Reflect(AZ::ReflectContext* context);
    };

    class VehicleControllerConfig final : public AZ::ComponentConfig
    {
    public:
        AZ_RTTI(VehicleControllerConfig, "{839DEE33-C909-4D83-A879-E3363548D975}");

        VehicleControllerConfig() = default;
        ~VehicleControllerConfig() = default;

        static void Reflect(AZ::ReflectContext* context);

        VehicleControllerServiceNames m_serviceNames;

        AZ::EntityId m_vehicleEntityId;
        float m_vehicleSpeed{ 1.0f };
        float m_vehicleSteeringGain{ 1.0f };
        float m_startDelay{ 1.0f };
        AZStd::vector<AZ::EntityId> m_predefinedPaths;
        AZStd::vector<AZ::EntityId> m_predefinedObstacles;
        AZStd::vector<AZ::EntityId> m_vehicleLights;
        AZStd::vector<float> m_vehicleLightsIntensities;
    };

} // namespace RAIControl