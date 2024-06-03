
#pragma once

#include <RAIAgricultureDemo/RAIAgricultureDemoTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace RAIAgricultureDemo
{
    class RAIAgricultureDemoRequests
    {
    public:
        AZ_RTTI(RAIAgricultureDemoRequests, RAIAgricultureDemoRequestsTypeId);
        virtual ~RAIAgricultureDemoRequests() = default;
        // Put your public methods here
    };

    class RAIAgricultureDemoBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using RAIAgricultureDemoRequestBus = AZ::EBus<RAIAgricultureDemoRequests, RAIAgricultureDemoBusTraits>;
    using RAIAgricultureDemoInterface = AZ::Interface<RAIAgricultureDemoRequests>;

} // namespace RAIAgricultureDemo
