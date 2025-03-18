#include <xar_engine/graphics/api/descriptor_set_reference.hpp>

#include <xar_engine/meta/enum_impl.hpp>


ENUM_TO_STRING_IMPL(xar_engine::graphics::api::EDescriptorType,
                    xar_engine::graphics::api::EDescriptorType::UNIFORM_BUFFER,
                    xar_engine::graphics::api::EDescriptorType::SAMPLED_IMAGE);