#pragma once

#include <set>

#include <xar_engine/graphics/api/buffer_reference.hpp>
#include <xar_engine/graphics/api/descriptor_pool_reference.hpp>
#include <xar_engine/graphics/api/descriptor_set_layout_reference.hpp>
#include <xar_engine/graphics/api/descriptor_set_reference.hpp>
#include <xar_engine/graphics/api/image_view_reference.hpp>
#include <xar_engine/graphics/api/sampler_reference.hpp>


namespace xar_engine::graphics::backend::component
{
    class IDescriptorComponent
    {
    public:
        struct MakeDescriptorPoolParameters;
        struct MakeDescriptorSetLayoutParameters;
        struct MakeDescriptorSetParameters;
        struct WriteDescriptorSetParameters;

    public:
        virtual ~IDescriptorComponent();


        virtual api::DescriptorPoolReference make_descriptor_pool(const MakeDescriptorPoolParameters& parameters) = 0;
        virtual api::DescriptorSetLayoutReference make_descriptor_set_layout(const MakeDescriptorSetLayoutParameters& parameters) = 0;
        virtual std::vector<api::DescriptorSetReference> make_descriptor_set_list(const MakeDescriptorSetParameters& parameters) = 0;

        virtual void write_descriptor_set(const WriteDescriptorSetParameters& parameters) = 0;
    };


    struct IDescriptorComponent::MakeDescriptorPoolParameters
    {
        std::set<api::EDescriptorType> descriptor_pool_type_list;
    };

    struct IDescriptorComponent::MakeDescriptorSetLayoutParameters
    {
        std::set<api::EDescriptorType> descriptor_pool_type_list;
    };

    struct IDescriptorComponent::MakeDescriptorSetParameters
    {
        api::DescriptorPoolReference descriptor_pool;
        api::DescriptorSetLayoutReference descriptor_set_layout;
        std::uint32_t descriptor_counts;
    };

    struct IDescriptorComponent::WriteDescriptorSetParameters
    {
        api::DescriptorSetReference& descriptor_set;
        std::uint32_t uniform_buffer_first_index;
        std::vector<api::BufferReference> uniform_buffer_list;
        std::uint32_t texture_image_first_index;
        std::vector<api::ImageViewReference> texture_image_view_list;
        std::vector<api::SamplerReference> sampler_list;
    };
}