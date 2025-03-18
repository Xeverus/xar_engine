#pragma once

#include <xar_engine/meta/enum.hpp>
#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class ImageTag;
    using ImageReference = meta::TResourceReference<ImageTag>;


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


ENUM_TO_STRING(xar_engine::graphics::api::EImageType);
ENUM_TO_STRING(xar_engine::graphics::api::EImageLayout);
ENUM_TO_STRING(xar_engine::graphics::api::EImageAspect);