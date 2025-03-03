#pragma once

#include <xar_engine/graphics/resource_reference.hpp>


namespace xar_engine::graphics
{
    enum class BufferTag;
    enum class CommandBufferTag;
    enum class DescriptorPoolTag;
    enum class DescriptorSetListTag;
    enum class DescriptorSetLayoutTag;
    enum class GrahicsPipelineTag;
    enum class ImageTag;
    enum class ImageViewTag;
    enum class QueueTag;
    enum class SamplerTag;
    enum class ShaderTag;
    enum class SurfaceTag;
    enum class SwapChainTag;

    using BufferReference = TResourceReference<BufferTag>;
    using CommandBufferReference = TResourceReference<CommandBufferTag>;
    using DescriptorPoolReference = TResourceReference<DescriptorPoolTag>;
    using DescriptorSetListReference = TResourceReference<DescriptorSetListTag>;
    using DescriptorSetLayoutReference = TResourceReference<DescriptorSetLayoutTag>;
    using GraphicsPipelineReference = TResourceReference<GrahicsPipelineTag>;
    using ImageReference = TResourceReference<ImageTag>;
    using ImageViewReference = TResourceReference<ImageViewTag>;
    using QueueReference = TResourceReference<QueueTag>;
    using SamplerReference = TResourceReference<SamplerTag>;
    using ShaderReference = TResourceReference<ShaderTag>;
    using SurfaceReference = TResourceReference<SurfaceTag>;
    using SwapChainReference = TResourceReference<SwapChainTag>;

    enum class EFormat
    {
        D32_SFLOAT,
        R32G32_SFLOAT,
        R32G32B32_SFLOAT,
        R8G8B8A8_SRGB,
    };

    enum class EImageType
    {
        COLOR_ATTACHMENT,
        DEPTH_ATTACHMENT,
        TEXTURE,
    };

    enum class EImageLayout
    {
        DEPTH_STENCIL_ATTACHMENT,
        TRANSFER_DESTINATION,
    };

    enum class EImageAspect
    {
        COLOR,
        DEPTH,
    };

    enum class ESwapChainResult
    {
        OK,
        RECREATION_REQUIRED,
        ERROR,
    };

    enum class ShaderType
    {
        VERTEX,
        FRAGMENT,
    };

    enum class VertexInputBindingRate
    {
        PER_VERTEX,
        PER_INSTANCE,
    };

    struct VertexInputAttribute
    {
        std::uint32_t binding_index;
        std::uint32_t location;
        std::uint32_t offset;
        EFormat format;
    };

    struct VertexInputBinding
    {
        std::uint32_t binding_index;
        std::uint32_t stride;
        VertexInputBindingRate input_rate;
    };

    enum ECommandBufferType
    {
        REUSABLE,
        ONE_TIME,
    };
}