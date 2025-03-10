#pragma once

#include <optional>
#include <string>


namespace xar_engine::asset
{
    struct Material
    {
        std::optional<std::string> color_base_texture;
    };
}