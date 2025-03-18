#include <xar_engine/graphics/api/swap_chain_reference.hpp>

#include <xar_engine/meta/enum_impl.hpp>


ENUM_TO_STRING_IMPL(xar_engine::graphics::api::ESwapChainResult,
                    xar_engine::graphics::api::ESwapChainResult::OK,
                    xar_engine::graphics::api::ESwapChainResult::RECREATION_REQUIRED,
                    xar_engine::graphics::api::ESwapChainResult::ERROR);