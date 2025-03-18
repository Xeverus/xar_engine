#include <xar_engine/graphics/api/image_reference.hpp>

#include <xar_engine/meta/enum_impl.hpp>


ENUM_TO_STRING_IMPL(xar_engine::graphics::api::EImageType,
                    xar_engine::graphics::api::EImageType::COLOR_ATTACHMENT,
                    xar_engine::graphics::api::EImageType::DEPTH_ATTACHMENT,
                    xar_engine::graphics::api::EImageType::TEXTURE);

ENUM_TO_STRING_IMPL(xar_engine::graphics::api::EImageLayout,
                    xar_engine::graphics::api::EImageLayout::DEPTH_STENCIL_ATTACHMENT,
                    xar_engine::graphics::api::EImageLayout::TRANSFER_DESTINATION);

ENUM_TO_STRING_IMPL(xar_engine::graphics::api::EImageAspect,
                    xar_engine::graphics::api::EImageAspect::COLOR,
                    xar_engine::graphics::api::EImageAspect::DEPTH);