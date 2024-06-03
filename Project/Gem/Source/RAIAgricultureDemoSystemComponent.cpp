
#include <AzCore/Serialization/SerializeContext.h>

#include "RAIAgricultureDemoSystemComponent.h"

#include <RAIAgricultureDemo/RAIAgricultureDemoTypeIds.h>

namespace RAIAgricultureDemo
{
    AZ_COMPONENT_IMPL(RAIAgricultureDemoSystemComponent, "RAIAgricultureDemoSystemComponent",
        RAIAgricultureDemoSystemComponentTypeId);

    void RAIAgricultureDemoSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<RAIAgricultureDemoSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void RAIAgricultureDemoSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("RAIAgricultureDemoService"));
    }

    void RAIAgricultureDemoSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("RAIAgricultureDemoService"));
    }

    void RAIAgricultureDemoSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void RAIAgricultureDemoSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    RAIAgricultureDemoSystemComponent::RAIAgricultureDemoSystemComponent()
    {
        if (RAIAgricultureDemoInterface::Get() == nullptr)
        {
            RAIAgricultureDemoInterface::Register(this);
        }
    }

    RAIAgricultureDemoSystemComponent::~RAIAgricultureDemoSystemComponent()
    {
        if (RAIAgricultureDemoInterface::Get() == this)
        {
            RAIAgricultureDemoInterface::Unregister(this);
        }
    }

    void RAIAgricultureDemoSystemComponent::Init()
    {
    }

    void RAIAgricultureDemoSystemComponent::Activate()
    {
        RAIAgricultureDemoRequestBus::Handler::BusConnect();
    }

    void RAIAgricultureDemoSystemComponent::Deactivate()
    {
        RAIAgricultureDemoRequestBus::Handler::BusDisconnect();
    }
}
