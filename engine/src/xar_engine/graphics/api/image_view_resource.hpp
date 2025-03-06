#pragma once

#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class ImageViewTag;
    using ImageViewReference = meta::TResourceReference<ImageViewTag>;
}