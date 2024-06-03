
#pragma once

#include <AzCore/Component/Component.h>

#include <RAIAgricultureDemo/RAIAgricultureDemoBus.h>

namespace RAIAgricultureDemo
{
    class RAIAgricultureDemoSystemComponent
        : public AZ::Component
        , protected RAIAgricultureDemoRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(RAIAgricultureDemoSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        RAIAgricultureDemoSystemComponent();
        ~RAIAgricultureDemoSystemComponent();

    protected:
        ////////////////////////////////////////////////////////////////////////
        // RAIAgricultureDemoRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
