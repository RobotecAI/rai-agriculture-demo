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
#include <rclcpp/rclcpp.hpp>
#include <std_srvs/srv/trigger.hpp>

namespace RAIControl
{
    class VehicleControllerComponent : public AZ::Component
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
        VehicleControllerConfig m_configuration;
    };
} // namespace RAIControl