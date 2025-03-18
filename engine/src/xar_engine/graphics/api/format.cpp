#include <xar_engine/graphics/api/format.hpp>

#include <xar_engine/meta/enum_impl.hpp>


ENUM_TO_STRING_IMPL(xar_engine::graphics::api::EFormat,
                    xar_engine::graphics::api::EFormat::D32_SIGNED_FLOAT,
                    xar_engine::graphics::api::EFormat::R32G32_SIGNED_FLOAT,
                    xar_engine::graphics::api::EFormat::R32G32B32_SIGNED_FLOAT,
                    xar_engine::graphics::api::EFormat::R8G8B8A8_SRGB);