
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include "RAIAgricultureDemoSystemComponent.h"
#include "VehicleController/VehicleControllerComponent.h"

#include <RAIAgricultureDemo/RAIAgricultureDemoTypeIds.h>

namespace RAIAgricultureDemo
{
    class RAIAgricultureDemoModule : public AZ::Module
    {
    public:
        AZ_RTTI(RAIAgricultureDemoModule, RAIAgricultureDemoModuleTypeId, AZ::Module);
        AZ_CLASS_ALLOCATOR(RAIAgricultureDemoModule, AZ::SystemAllocator);

        RAIAgricultureDemoModule()
            : AZ::Module()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(
                m_descriptors.end(),
                { RAIAgricultureDemoSystemComponent::CreateDescriptor(), RAIControl::VehicleControllerComponent::CreateDescriptor() });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<RAIAgricultureDemoSystemComponent>(),
            };
        }
    };
} // namespace RAIAgricultureDemo

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), RAIAgricultureDemo::RAIAgricultureDemoModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_RAIAgricultureDemo, RAIAgricultureDemo::RAIAgricultureDemoModule)
#endif
