//  Copyright (c) 2011      Bryce Lelbach
//  Copyright (c) 2007-2011 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_F5D19D10_9D74_4DB9_9ABB_ECCF2FA54497)
#define HPX_F5D19D10_9D74_4DB9_9ABB_ECCF2FA54497

#include <hpx/hpx_fwd.hpp>
#include <hpx/runtime/components/component_type.hpp>
#include <hpx/runtime/parcelset/parcelhandler.hpp>
#include <hpx/runtime/naming/name.hpp>
#include <hpx/runtime/naming/address.hpp>
#include <hpx/runtime/applier/applier.hpp> 
#include <hpx/runtime/applier/bind_naming_wrappers.hpp> 
#include <hpx/util/static.hpp>
#include <hpx/util/stringstream.hpp>

namespace hpx { namespace components 
{

template <boost::uint64_t MSB, boost::uint64_t LSB>
struct gid_tag;

///////////////////////////////////////////////////////////////////////////
/// \class fixed_component_base fixed_component_base.hpp hpx/runtime/components/server/fixed_component_base.hpp
template <boost::uint64_t MSB, boost::uint64_t LSB, typename Component>
struct fixed_component_base : detail::fixed_component_tag
{
  private:
    typedef typename boost::mpl::if_<
            boost::is_same<Component, detail::this_type>, 
            fixed_component_base, Component
        >::type this_component_type;
    
  public:
    typedef this_component_type wrapped_type;
    typedef this_component_type base_type_holder;

    /// \brief Construct an empty fixed_component
    fixed_component_base()
    {}

    /// \brief Destruct a fixed_component
    ~fixed_component_base()
    {
        if (gid_ != fixed_gid)
            applier::unbind_gid(gid_); 
    }
 
    /// \brief finalize() will be called just before the instance gets 
    ///        destructed
    void finalize() {}

    // \brief This exposes the component type. 
    static component_type get_component_type()
    {
        return components::get_component_type<this_component_type>();
    }
    static void set_component_type(component_type type)
    {
        components::set_component_type<this_component_type>(type);
    }

    /// \brief Return the component's fixed GID. 
    ///
    /// \returns      The fixed global id (GID) for this component
    naming::gid_type const& get_base_gid() const
    {
        if (!gid_)
        {
            naming::address addr(applier::get_applier().here(),
                components::get_component_type<wrapped_type>(),
                hpx::uintptr_t(static_cast<this_component_type const&>(this)));

            gid_ = fixed_gid();

            // Try to bind the preset GID first
            if (!applier::bind_gid(gid_, addr))
            {
                gid_ = applier::get_applier().get_parcel_handler().get_next_id();
              
                // If we can't bind the preset GID, then try to bind the next
                // available GID on this locality. 
                if (!applier::bind_gid(gid_, addr))
                { 
                    hpx::util::osstream strm;
                    strm << gid_;
                    gid_ = naming::gid_type();   // invalidate GID
                    HPX_THROW_EXCEPTION(duplicate_component_address,
                        "fixed_component_base<Component>::get_base_gid", 
                        hpx::util::osstream_get_string(strm));
                }
            }
        }
        return gid;
    }

    static naming::gid_type fixed_gid() const
    {
        util::static_<naming::gid_type, gid_tag<MSB, LSB>, 1>
            fixed(naming::gid_type(MSB, LSB));

        return fixed;
    }

    /// \brief  The function \a get_factory_properties is used to 
    ///         determine, whether instances of the derived component can 
    ///         be created in blocks (i.e. more than one instance at once). 
    ///         This function is used by the \a distributing_factory to 
    ///         determine a correct allocation strategy
    static factory_property get_factory_properties()
    {
        // components derived from this template have to be allocated one
        // at a time
        return factory_none;
    }

  private:
    bool try_bind_gid() const
    {

    }

    mutable naming::gid_type gid_;
};


///////////////////////////////////////////////////////////////////////////
/// \class fixed_component fixed_component.hpp hpx/runtime/components/server/fixed_component_base.hpp
template <typename Component>
struct fixed_component : Component
{
    typedef Component type_holder;

    /// \brief  The function \a create is used for allocation and 
    ///         initialization of instances of the derived components.
    static Component* create(std::size_t count)
    {
        // fixed components can be created individually only
        BOOST_ASSERT(1 == count);
        return new Component();
    }

    /// \brief  The function \a destroy is used for destruction and 
    ///         de-allocation of instances of the derived components.
    static void destroy(Component* p, std::size_t count = 1)
    {
        // fixed components can be deleted individually only
        BOOST_ASSERT(1 == count);
        p->finalize();
        delete p;
    }
};

}}

#endif // HPX_F5D19D10_9D74_4DB9_9ABB_ECCF2FA54497

