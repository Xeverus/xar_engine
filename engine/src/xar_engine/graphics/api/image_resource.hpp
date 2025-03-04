#pragma once

#include <xar_engine/graphics/api/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class ImageTag;
    using ImageReference = TResourceReference<ImageTag>;


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
}