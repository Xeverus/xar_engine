#pragma once

#include <cstdint>
#include <unordered_map>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::meta
{
    template <typename Tag,
              typename Type>
    class TResourceMap
    {
    public:
        using Resource = TResourceReference<Tag>;
        using ResourceId = Resource::Id;

    public:
        TResourceMap();

        Resource add(Type&& object);

        const Type& get(const Resource& resource) const;
        Type& get(const Resource& resource);

        [[nodiscard]]
        std::size_t size() const;

    private:
        std::unordered_map<ResourceId, Type> _resource_map;
        ResourceId _next_id;
    };


    template <typename Tag,
              typename Type>
    TResourceMap<Tag, Type>::TResourceMap()
        : _resource_map{}
        , _next_id(0)
    {
    }

    template <typename Tag,
              typename Type>
    TResourceMap<Tag, Type>::Resource TResourceMap<Tag, Type>::add(Type&& object)
    {
        const auto resource_id = _next_id++;
        _resource_map.emplace(
            resource_id,
            std::forward<Type>(object));

        return Resource{
            resource_id, [this, resource_id]()
            {
                _resource_map.erase(resource_id);
            }};
    }

    template <typename Tag,
              typename Type>
    const Type& TResourceMap<Tag, Type>::get(const Resource& resource) const
    {
        const auto iter = _resource_map.find(resource.get_id());
        XAR_THROW_IF(
            iter == _resource_map.end(),
            error::XarException,
            "{} has no resource with id {}",
            XAR_OBJECT_ID(this),
            resource.get_id());

        return iter->second;
    }

    template <typename Tag,
              typename Type>
    Type& TResourceMap<Tag, Type>::get(const Resource& resource)
    {
        const auto iter = _resource_map.find(resource.get_id());
        XAR_THROW_IF(
            iter == _resource_map.end(),
            error::XarException,
            "{} has no resource with id {}",
            XAR_OBJECT_ID(this),
            resource.get_id());

        return iter->second;
    }

    template <typename Tag,
              typename Type>
    std::size_t TResourceMap<Tag, Type>::size() const
    {
        return _resource_map.size();
    }
}