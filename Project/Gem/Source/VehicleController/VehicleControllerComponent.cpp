/*
 * Copyright (c) Robotec.ai.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "VehicleControllerComponent.h"
#include <AtomLyIntegration/CommonFeatures/CoreLights/AreaLightBus.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <LmbrCentral/Shape/SplineComponentBus.h>
#include <ROS2/Frame/ROS2FrameComponent.h>
#include <ROS2/ROS2Bus.h>
#include <ROS2/RobotControl/Ackermann/AckermannBus.h>
#include <ROS2/RobotControl/Ackermann/AckermannCommandStruct.h>
#include <ROS2/Utilities/ROS2Names.h>

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

        auto ros2Frame = ROS2::Utils::GetGameOrEditorComponent<ROS2::ROS2FrameComponent>(GetEntity());
        if (!ros2Frame)
        {
            AZ_Error("VehicleControllerComponent", false, "ROS2 frame is not available. ROS 2 services will not be created.");
            return;
        }

        const auto& serviceNames = m_configuration.m_serviceNames;

        AZStd::string serviceName = ROS2::ROS2Names::GetNamespacedName(ros2Frame->GetNamespace(), serviceNames.m_continueServiceName);
        m_continueService = ros2Node->create_service<std_srvs::srv::Trigger>(
            serviceName.c_str(),
            [this]([[maybe_unused]] const TriggerSrvRequest request, TriggerSrvResponse response)
            {
                m_currentState = VehicleState::DRIVING;
                response->success = true;
                response->message = "The vehicle continues the mission";
            });

        serviceName = ROS2::ROS2Names::GetNamespacedName(ros2Frame->GetNamespace(), serviceNames.m_replanServiceName);
        m_replanService = ros2Node->create_service<std_srvs::srv::Trigger>(
            serviceName.c_str(),
            [this]([[maybe_unused]] const TriggerSrvRequest request, TriggerSrvResponse response)
            {
                m_currentState = VehicleState::REVERSING;
                response->success = true;
                response->message = "The current mission was aborted";
            });

        serviceName = ROS2::ROS2Names::GetNamespacedName(ros2Frame->GetNamespace(), serviceNames.m_stopServiceName);
        m_stopService = ros2Node->create_service<std_srvs::srv::Trigger>(
            serviceName.c_str(),
            [this]([[maybe_unused]] const TriggerSrvRequest request, TriggerSrvResponse response)
            {
                m_currentState = VehicleState::STOPPED;
                response->success = true;
                response->message = "The vehicle was stopped";
            });

        serviceName = ROS2::ROS2Names::GetNamespacedName(ros2Frame->GetNamespace(), serviceNames.m_stateServiceName);
        m_stateService = ros2Node->create_service<std_srvs::srv::Trigger>(
            serviceName.c_str(),
            [this]([[maybe_unused]] const TriggerSrvRequest request, TriggerSrvResponse response)
            {
                static AZStd::unordered_map<VehicleState, AZStd::string> stateMessages = {
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

        serviceName = ROS2::ROS2Names::GetNamespacedName(ros2Frame->GetNamespace(), serviceNames.m_flashServiceName);
        m_flashService = ros2Node->create_service<std_srvs::srv::Trigger>(
            serviceName.c_str(),
            [this]([[maybe_unused]] const TriggerSrvRequest request, TriggerSrvResponse response)
            {
                if (m_configuration.m_vehicleLights.size() != m_configuration.m_vehicleLightsIntensities.size())
                {
                    response->success = false;
                    response->message = "Cannot flash, lights configured incorrectly.";
                }
                else
                {
                    m_switchLights = true;
                    response->success = true;
                    response->message = "Flashing triggered.";
                }
            });

        AZ::TickBus::Handler::BusConnect();
    }

    void VehicleControllerComponent::Deactivate()
    {
        m_continueService.reset();
        m_replanService.reset();
        m_stopService.reset();
        m_stateService.reset();
        m_flashService.reset();

        if (AZ::TickBus::Handler::BusIsConnected())
        {
            AZ::TickBus::Handler::BusDisconnect();
        }
    }
    void VehicleControllerComponent::LightsOn()
    {
        const AZ::Render::PhotometricUnit photoUnit = AZ::Render::PhotometricUnit::Lumen;

        auto it1 = m_configuration.m_vehicleLights.begin();
        auto it2 = m_configuration.m_vehicleLightsIntensities.begin();
        for (; it1 != m_configuration.m_vehicleLights.end() && it2 != m_configuration.m_vehicleLightsIntensities.end(); ++it1, ++it2)
        {
            AZ::Render::AreaLightRequestBus::Event(*it1, &AZ::Render::AreaLightRequests::SetIntensityAndMode, *it2, photoUnit);
        }
    }

    void VehicleControllerComponent::LightsOff()
    {
        const AZ::Render::PhotometricUnit photoUnit = AZ::Render::PhotometricUnit::Lumen;

        for (const auto& lightId : m_configuration.m_vehicleLights)
        {
            AZ::Render::AreaLightRequestBus::Event(lightId, &AZ::Render::AreaLightRequests::SetIntensityAndMode, 0.0f, photoUnit);
        }
    }

    AZStd::pair<float, float> VehicleControllerComponent::MoveVehicle()
    {
        AZ::Vector3 vehiclePosition = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(vehiclePosition, m_configuration.m_vehicleEntityId, &AZ::TransformBus::Events::GetWorldTranslation);
        DetectCollisions(vehiclePosition);

        AZ::Transform vehiclePose = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(vehiclePose, m_configuration.m_vehicleEntityId, &AZ::TransformBus::Events::GetWorldTM);
        const AZ::Transform vehiclePoseInv = vehiclePose.GetInverse();

        static AZStd::unordered_map<VehicleState, float> directions = { { VehicleState::STOPPED, 0.0f },
                                                                        { VehicleState::DRIVING, 1.0f },
                                                                        { VehicleState::REVERSING, -1.0f },
                                                                        { VehicleState::CHANGING_PATH, 1.0f },
                                                                        { VehicleState::FINISHED, 0.0f } };

        // Assume X axis looking forward
        const AZ::Vector3 vehicleForward = AZ::Vector3::CreateAxisX() * directions[m_currentState];
        const AZ::Vector3 probePosition = vehiclePosition + vehiclePose.TransformVector(vehicleForward);

        AZ::Transform splinePose = AZ::Transform::CreateIdentity();
        const auto& splineEntityId = m_configuration.m_predefinedPaths[m_currentPath];
        AZ::TransformBus::EventResult(splinePose, splineEntityId, &AZ::TransformBus::Events::GetWorldTM);
        const AZ::Transform splinePoseInv = splinePose.GetInverse();

        AZ::ConstSplinePtr splinePtr{ nullptr };
        LmbrCentral::SplineComponentRequestBus::EventResult(splinePtr, splineEntityId, &LmbrCentral::SplineComponentRequests::GetSpline);
        if (!splinePtr)
        {
            AZ_Error("VehicleControllerComponent", false, "Cannot find spline to calculate the vehicle's movement.");
            return {};
        }

        const AZ::Vector3 probePositionInSplineFrame = splinePoseInv.TransformPoint(probePosition);
        const auto nearestAddress = splinePtr->GetNearestAddressPosition(probePositionInSplineFrame);
        const AZ::Vector3 nearestPosition = splinePtr->GetPosition(nearestAddress.m_splineAddress);
        const AZ::Vector3 nearestPositionWorld = splinePose.TransformPoint(nearestPosition);
        const AZ::Vector3 nearestPositionVehicleFrame = vehiclePoseInv.TransformPoint(nearestPositionWorld);

        // Assume X axis looking forward, Y axis looking left (-Y looking right)
        const float crossTrackError = nearestPositionVehicleFrame.Dot(-AZ::Vector3::CreateAxisY());
        const float lateralError = nearestPositionVehicleFrame.Dot(AZ::Vector3::CreateAxisX()) - directions[m_currentState];

        float steeringAngle = -m_configuration.m_vehicleSteeringGain * crossTrackError;
        const float speed = m_configuration.m_vehicleSpeed * directions[m_currentState];

        // Reduce steering for large distances (to eliminate "zig-zag" movement)
        if (lateralError > 1.0f)
        {
            steeringAngle /= lateralError;
        }

        // Apply Ackermann steering
        ROS2::AckermannCommandStruct acs;
        acs.m_steeringAngle = steeringAngle;
        acs.m_steeringAngleVelocity = 0.0f;
        acs.m_speed = speed;
        acs.m_acceleration = 0.0f;
        acs.m_jerk = 0.0f;
        ROS2::AckermannNotificationBus::Event(m_configuration.m_vehicleEntityId, &ROS2::AckermannNotifications::AckermannReceived, acs);

        return AZStd::make_pair(crossTrackError, lateralError);
    }

    void VehicleControllerComponent::DetectCollisions(const AZ::Vector3& vehiclePosition)
    {
        for (const auto& obstacleId : m_configuration.m_predefinedObstacles)
        {
            AZ::Vector3 obstacleTranslation = AZ::Vector3::CreateZero();
            AZ::TransformBus::EventResult(obstacleTranslation, obstacleId, &AZ::TransformBus::Events::GetWorldTranslation);

            if ((obstacleTranslation - vehiclePosition).GetLength() < 5.0)
            {
                // Set the flag and stop the vehicle
                if (!m_obstacleDetected)
                {
                    m_obstacleDetected = true;
                    m_currentState = VehicleState::STOPPED;
                }
                // No need to look for other collisions
                return;
            }
        }
        // No collisions detected; reset the flag
        m_obstacleDetected = false;
    }

    void VehicleControllerComponent::SwitchPath()
    {
        if (m_currentPath < m_configuration.m_predefinedPaths.size() - 1)
        {
            m_currentState = VehicleState::CHANGING_PATH;
            ++m_currentPath;
        }
        else
        {
            m_currentState = VehicleState::FINISHED;
        }
    }

    void VehicleControllerComponent::OnTick(float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint timePoint)
    {
        if (deltaTime > 0.25f)
        {
            return;
        }

        // startup delay
        m_currentTime += deltaTime;
        if (m_currentTime < m_configuration.m_startDelay)
        {
            return;
        }

        // switch on/off lights
        if (m_switchLights)
        {
            LightsOn();
            m_switchLights = false;
            m_lightsTime = m_currentTime + 0.2f; // keep the lights on for 200ms
        }
        if (m_lightsTime < m_currentTime)
        {
            LightsOff();
            m_lightsTime = AZStd::numeric_limits<float>::infinity();
        }

        // early terminate
        if (m_currentState == VehicleState::FINISHED || m_currentState == VehicleState::STOPPED)
        {
            return;
        }

        // calculate movement and process state machine
        const auto [crossTrackError, lateralError] = MoveVehicle();
        if (m_currentState == VehicleState::DRIVING && lateralError < -0.5)
        {
            // Reached the end of the current path
            SwitchPath();
        }
        else if (m_currentState == VehicleState::REVERSING && lateralError > 0.5)
        {
            // Reaching the beginning of the aborted path
            SwitchPath();
        }
        else if (m_currentState == VehicleState::CHANGING_PATH && AZStd::abs(lateralError) < 0.5 && AZStd::abs(crossTrackError) < 0.5)
        {
            // Reached the next path, continue driving on a path
            m_currentState = VehicleState::DRIVING;
        }
    }
} // namespace RAIControl
